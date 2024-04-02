#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int pid = atoi(argv[1]);

    FILE *topOutput = popen("top -bn1 | grep '^%Cpu' | tail -n1 | awk '{print $2}'", "r");
    if (topOutput == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    float porcentaje = 0.0;
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, BUFFER_SIZE, topOutput) != NULL) {
        porcentaje = atof(buffer);
    }
    pclose(topOutput);

    printf("%.2f\n", porcentaje);
    
    /*
    char command[256]; // Tamaño suficiente para almacenar el comando
    snprintf(command, sizeof(command), "/usr/bin/pidstat -p %s -u 1 5 | tail -n 1 | awk '{print $8}'", argv[1]);

    FILE *pidstatOutput = popen(command, "r");
    if (pidstatOutput == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    float percentage = 0.0;
    char bufferPID[BUFFER_SIZE];
    if (fgets(bufferPID, BUFFER_SIZE, pidstatOutput) != NULL) {
        percentage = atof(bufferPID);
    }
    pclose(pidstatOutput);

    printf("CPU Utilization for PID %d over the last 5 minutes: %.2f%%\n", pid, percentage);
*/

    FILE *file;
    char filename[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    long utime, stime, cutime, cstime;
    double total_time;

    // Construye la ruta al archivo /proc/[PID]/stat
    snprintf(filename, sizeof(filename), "/proc/%d/stat", pid);

    // Abre el archivo
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    // Lee la línea del archivo
    fgets(line, sizeof(line), file);

    // Cierra el archivo
    fclose(file);

    // Extrae los campos necesarios
    char *token = strtok(line, " ");
    for (int i = 1; i <= 13; ++i) {
        token = strtok(NULL, " ");
    }
    utime = atol(token);
    token = strtok(NULL, " ");
    stime = atol(token);
    token = strtok(NULL, " ");
    cutime = atol(token);
    token = strtok(NULL, " ");
    cstime = atol(token);

    // Calcula el tiempo total de CPU utilizado
    total_time = utime + stime + cutime + cstime;
    printf("CPU Utilization for PID %d over the last 5 minutes: %.2f%%\n", pid, total_time);
    return 0;
}

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double getTotalCpuUtilization() {
    FILE *proc_stat;
    char buffer[1024];
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    proc_stat = fopen("/proc/stat", "r");
    if (!proc_stat) {
        perror("Failed to open /proc/stat");
        return -1.0; // Error indicator
    }

    fgets(buffer, sizeof(buffer), proc_stat);
    fclose(proc_stat);

    // Parse the CPU statistics
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    // Calculate total CPU time
    long totalTime = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;

    // Calculate idle time
    long idleTime = idle + iowait;

    // Calculate total CPU utilization
    double totalCpuUtilization = 100.0 * (1.0 - ((double)idleTime / totalTime));

    return totalCpuUtilization;
}

int main() {
    double cpu_utilization = getTotalCpuUtilization();
    if (cpu_utilization != -1.0) {
        printf("Total CPU utilization: %.2f%%\n", cpu_utilization);
    }
    return 0;
}
*/