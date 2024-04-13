#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double getTotalCpuUtilization() {
    FILE *proc_stat;
    char buffer[512];
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

    proc_stat = fopen("/proc/stat", "r");
    if (!proc_stat) {
        perror("Failed to open /proc/stat");
        return -1.0; // Error indicator
    }

    fgets(buffer, sizeof(buffer), proc_stat);
    fclose(proc_stat);

    // Retrieve the CPU statistics
    sscanf(buffer, "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    // Total CPU time
    long totalTime = user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;

    // Idle time
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
