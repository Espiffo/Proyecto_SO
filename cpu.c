#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#define PROC_STAT "/proc/stat"
#define PROC_PID_SCHED "/proc/%d/sched"
#define PROC_PID_STAT  "/proc/%d/stat"
#define PROC_UPTIME "/proc/uptime"


double getSystemUptime();
double getTotalCpuUtilization();
void getCpuTimes(long *, long *);
int wasExecutedInLastFiveMinutes(int);
int getProcessTimes(int pide, long *, long *);
int getProcessNameByPid(int, char *, size_t);

int main(int args, char *argv[]) {
  
   if(args == 1)
   {
        double cpu_utilization = getTotalCpuUtilization();
        if (cpu_utilization != -1.0) {
            printf("Utilización total del CPU: %.2f%%\n", cpu_utilization);
        }else{
            return 1;
        }

    }else if(args == 2)
    {
        int pid;
        if((pid = atoi(argv[1])) == 0){
            fprintf(stderr, "Error de parámetros: error al convertir valores\n");
            exit(EXIT_FAILURE);
            return 1;
        }
        
        char nombre[256];
        if(getProcessNameByPid(pid, nombre, sizeof(nombre)) != 0){
            fprintf(stderr, "Error de parámetros: el proceso no existe\n");
            exit(EXIT_FAILURE);
            return 1;
        }
        
        if(wasExecutedInLastFiveMinutes(pid) == 1){

            printf("Proceso: %s\nPID: %d\nHa sido ejecutado en los últimos 5 minutos\n", nombre, pid);

        }else{

            printf("Proceso: %s\n\nPID: %d\nNo ha sido ejecutado en los últimos 5 minutos\n", nombre, pid);

        }

    }
    
    return 0;
}

double getSystemUptime(){
    FILE *fp = fopen(PROC_UPTIME, "r");
    if (!fp) {
        perror("Failed to open /proc/uptime");
        return -1; 
    }

    double uptime_seconds;
    if (fscanf(fp, "%lf", &uptime_seconds) != 1) {
        perror("Failed to read uptime");
        fclose(fp);
        return -1; 
    }

    fclose(fp);
    return uptime_seconds;
}

double getTotalCpuUtilization() {
    
    long idleTime, totalTime;
    getCpuTimes(&totalTime, &idleTime);

    // Calculate total CPU utilization
    double totalCpuUtilization = 100.0 * (1.0 - ((double)idleTime / totalTime));

    return totalCpuUtilization;
}

void getCpuTimes(long *total_time, long *idle_time){
    FILE *proc_stat;
    char buffer[512];
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    proc_stat = fopen(PROC_STAT, "r");
    if (!proc_stat) {
        perror("Failed to open /proc/stat");
        return; 
    }

    fgets(buffer, sizeof(buffer), proc_stat);
    fclose(proc_stat);

    // Retrieve the CPU statistics
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    // Total CPU time
    *total_time = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;

    // Idle time
    *idle_time = idle + iowait;
}

int getProcessTimes(int pid, long * usr, long * sys){
    char path[256];
    FILE *file;

    // Build the path to the process's stat file
    sprintf(path, PROC_PID_STAT, pid);
    file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open process stat file");
        return -1; // Error opening file
    }

    // Skip the first 13 fields and then read the 14th (utime) and 15th (stime)
    if (fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %ld %ld",
               usr, sys) != 2) {
        perror("Failed to read user and system time");
        fclose(file);
        return -1; // Error reading values
    }

    fclose(file);
    return 0; // Success
}

int wasExecutedInLastFiveMinutes(int pid) {
    char path[256];
    FILE *fp;
    char line[1024];
    double last_exec_start; //milliseconds
    struct timespec now; //seconds
    double now_in_ns;
    double five_minutes_in_ns = 300 * 1e9; // 300 seconds in nanoseconds

    sprintf(path, PROC_PID_SCHED, pid);
    fp = fopen(path, "r");
    if (!fp) {
        perror("Failed to open sched file");
        return -1;  // Error indicator
    }

    // Read the sched file and find the line containing 'se.exec_start'
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "se.exec_start")) {
            sscanf(line, "se.exec_start : %lf", &last_exec_start);
            fclose(fp);
            clock_gettime(CLOCK_BOOTTIME, &now); //retrieve the time since boot in seconds
            now_in_ns = now.tv_sec * 1e9 + now.tv_nsec; //conversion from seconds to nanoseconds 
            double last_exec_start_ns = last_exec_start * 1e6; //conversion from milliseconds to nanoseconds
            double time_since_last_exec = now_in_ns - last_exec_start_ns;
            
            // Check if the process was executed in the last 5 minutes
            if (time_since_last_exec <= five_minutes_in_ns) {
                return 1;  // Process was active in the last five minutes
            } else {
                return 0;  // Process was not active in the last five minutes
            }
        }
    }
    fclose(fp);
    return 0;  // se.exec_start not found
}

int getProcessNameByPid(int pid, char *processName, size_t nameSize) {
    char path[256];
    FILE *file;

    // Construct the path to the /proc/[pid]/comm file
    snprintf(path, sizeof(path), "/proc/%d/comm", pid);
    
    // Open the comm file of the process
    file = fopen(path, "r");
    if (file == NULL) {
        perror("Failed to open comm file");
        return -1;  // Return -1 on failure
    }

    // Read the process name from the file
    if (fgets(processName, nameSize, file) == NULL) {
        perror("Failed to read from comm file");
        fclose(file);
        return -1;  // Return -1 on failure
    }

    // Remove the newline character at the end of the process name
    size_t len = strlen(processName);
    if (len > 0 && processName[len - 1] == '\n') {
        processName[len - 1] = '\0';
    }

    fclose(file);
    return 0;  // Return 0 on success
}