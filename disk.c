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

#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>


#define BUFFER_SIZE 256


char* used_space(char*, struct statvfs);
char* free_space(char*, struct statvfs);
double bytes_to_MiB(unsigned long);
double bytes_to_GiB(unsigned long);

int main(int argc, char* argv[]) {
    // Definir la ruta del sistema de archivos montado en el disco sda
    const char *mount_point = "/";

    // Estructura para almacenar información sobre el sistema de archivos
    struct statvfs fs_info;

    // Obtener información sobre el sistema de archivos
    if (statvfs(mount_point, &fs_info) == -1) {
        perror("Error al obtener información del sistema de archivos");
        return 1;
    }

    char* resultado = free_space(argv[1], fs_info);

    // Mostrar el resultado del espacio libre
    printf("%s", resultado);
    
    free(resultado);
    
    resultado = used_space(argv[1], fs_info);

 // Mostrar el resultado del espacio utilizado
    printf("%s", resultado);
    
    free(resultado);

    return 0;
}

char* used_space(char* opcion, struct statvfs fs_info){

    // Asignar espacio en memoria para la cadena de caracteres
    char* str = malloc(sizeof(char) * BUFFER_SIZE);
    
    // Calcular el espacio total en bytes
    unsigned long long total_space_bytes = fs_info.f_blocks * fs_info.f_bsize;
    
    // Calcular el espacio utilizado en bytes
    unsigned long long used_space_bytes = total_space_bytes - (fs_info.f_bfree * fs_info.f_bsize);
    double used_space = 0;
    
    
    if(strcmp(opcion, "-tm") == 0){
    	//Convertir bytes a MiB
    	used_space = bytes_to_MiB(used_space_bytes); 
    	
    	//Construir cadena
    	snprintf(str, BUFFER_SIZE, "El espacio utilizado en el disco duro es de aproximadamente %.2f MiB.\n", used_space);
    	
    }else if(strcmp(opcion, "-tg") == 0){
    	//Convertir bytes a GiB
    	used_space = bytes_to_GiB(used_space_bytes); 
    	
    	//Construir cadena
    	snprintf(str, BUFFER_SIZE, "El espacio utilizado en el disco duro es de aproximadamente %.2f GiB.\n", used_space);
    }
    
    return str;
}

char* free_space(char* opcion, struct statvfs fs_info){

    // Asignar espacio en memoria para la cadena de caracteres
    char* str = malloc(sizeof(char) * BUFFER_SIZE);
    
    // Calcular el espacio libre en bytes
    unsigned long long free_space_bytes = fs_info.f_bfree * fs_info.f_bsize;
    
    
    double free_space = 0;
    
    
    if(strcmp(opcion, "-tm") == 0){
    	//Convertir bytes a MiB
    	free_space = bytes_to_MiB(free_space_bytes);
    	
    	//Construir cadena
    	snprintf(str, BUFFER_SIZE, "El espacio libre en el disco duro es de aproximadamente %.2f MiB.\n", free_space);
    	
    }else if(strcmp(opcion, "-tg") == 0){
    	//Convertir bytes a GiB
    	free_space = bytes_to_GiB(free_space_bytes);
    	
    	//Construir cadena
    	snprintf(str, BUFFER_SIZE, "El espacio libre en el disco duro es de aproximadamente %.2f GiB.\n", free_space);
    }
    
    return str;
}

double bytes_to_MiB(unsigned long bytes){
     return (double)bytes / (1024 * 1024);
}
double bytes_to_GiB(unsigned long bytes){
    return (double)bytes / (1024 * 1024 * 1024);
}
