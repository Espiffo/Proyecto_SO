#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define PATH_MAX_LENGTH 256
//por que 256?
// 255 es la longitud maxima del nombre un archivo en LINUX
//
long obtener_memoria_virtual(int pid) {
    char path[PATH_MAX_LENGTH];
    FILE *status_file;
    char line[256];
	//para guardar linea por linea, el max de una linea en LINUX 
	//es 255 char's de ahi el nombre
    long memoria_virtual_kb = -1;

    // el path como tal se guarda en el arreglo de char's (buffer), "path"
    snprintf(path, PATH_MAX_LENGTH, "/proc/%d/status", pid);
  //snprintf(path, PATH_MAX_LENGTH, "/proc/%d/status", pid);

    status_file = fopen(path, "r");
    if (status_file == NULL) {
        perror("! open status");
        return -1;
    }

    // linea X linea no importa el tamano, por eso se usa: line[256]
    while (fgets(line, sizeof(line), status_file)) {
        // Traduccion: si los primeros 7 char's son ==, entre al ->if<-
        if (strncmp(line, "VmSize:", 7) == 0) {
             //Saca el tamaño en kilobytes 
	    //%=Okay "sscanf" le voy a dar un formato para que interprete
	   //*=Okay "sscanf" lo siguinete lo va a ignorar
	  //s=Okay "sscanf" ignore \n y \t
	 //Okay y guarde eso en memoria_virtual_kb
            sscanf(line, "%*s %ld", &memoria_virtual_kb);
            break;
        }
    }
    fclose(status_file);
    return memoria_virtual_kb;
}

int main() {
    FILE *proc_file;
    char path[PATH_MAX_LENGTH];
    int pid;
    long total_memoria_fisica_kb;
    long memoria_virtual_kb;
    char nombre_proceso[256];

    // Abrir el archivo /proc/meminfo para obtener la memoria física total
    proc_file = fopen("/proc/meminfo", "r");
  //proc_file = fopen("cd ../../proc/meminfo", "r");
    if (proc_file == NULL) {
        perror("! open meminfo");
        return 1;
    }

    // %ld para leer long int
    fscanf(proc_file, "%*s %ld", &total_memoria_fisica_kb);

    
    fclose(proc_file);
    printf("[Total de memoria fisica: %ld KB]\n", total_memoria_fisica_kb);

    // Obtener el PID de este programa
   
           //por que el actual PID de este procesos (de este codigo) es el MAX?

          //porque sino se define un maximo entonces el programa nunca va a terminar, seria un 
         //bucle infinito ya que durante la ejecucion pueden seguiry segir salindo procesos mayores
	//al actual y por ende nunca saldria del bucle.
    printf("------------------------------------------------------------------\n\n");
    printf("PID\t Nombre\t \t Porcentaje de Utilización de Memoria Virtual\n\n");
    printf("------------------------------------------------------------------\n\n");

    DIR *proc_dir = opendir("/proc");
    struct dirent *entry; //se usa para reprecentar el dir

    
  	if (proc_dir == NULL) {
  		perror("! open /proc");
  		return 1;
  	}
    //okay readdir hace eso mismo read dir y regrsa un puntero hacia el sigient sub dir de /proc
    //es como iterar en el dir /proc los dir que tiene dentro  
    while((entry=readdir(proc_dir))!=NULL){
        if(entry->d_type==DT_DIR){
            int pid =atoi(entry->d_name);
            if(pid!=0){
                printf("PID: %d\n",pid);
                memoria_virtual_kb = obtener_memoria_virtual(pid);
                    if (memoria_virtual_kb != -1) {
                        //comm: para sacar el nombre del proceso
                        snprintf(path, PATH_MAX_LENGTH, "/proc/%d/comm", pid);
                        proc_file = fopen(path, "r");
                        if (proc_file != NULL) {
                            fgets(nombre_proceso, sizeof(nombre_proceso), proc_file);
                            fclose(proc_file);
                            // Calcular el porcentaje de utilización de memoria virtual
                            double porcentaje_utilizacion = (double)memoria_virtual_kb / total_memoria_fisica_kb * 100.0;
                            // Imprimir la información del proceso
                            printf(" %d\t | %s\t | %.2f%%\n", pid, nombre_proceso, porcentaje_utilizacion);
                        }
                    }
            }
        }
    }
    closedir(proc_dir);


    return 0;
}
