#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define longitudmax 256
//por que 256?
// 255 es la longitud maxima del nombre un archivo en LINUX
//
long mv_pid(int pid) {
    char path[longitudmax];
    FILE *archistatus;
    char L[256];
	//para guardar linea por liena, el max de una liena en LINUX 
	//es 255 char's de ahi el proque 256 
    long mv = -1;

    // el path como tal se guarda en el arreglo de char's (buffer), "path"
    snprintf(path, longitudmax, "/proc/%d/status", pid);
  //snprintf(path, longitudmax, "/proc/%d/status", pid);

    //la longitud del path es como para prever un nombre super largo 
    archistatus = fopen(path, "r");
    if (archistatus == NULL) {
        perror("! open status");
        return -1;
    }

    // linea X liena a no importa el tamano, por eso se usa: L[256]
    while (fgets(L, sizeof(L), archistatus)) {
        // Traduccion: si los primeros 7 char's son ==, entre al ->if<-
        if (strncmp(L, "VmSize:", 7) == 0) {
             //Saca el tamaño en kilobytes 
	    //%=Okay "sscanf" le voy a dar un formato para que interprete
	   //*=Okay "sscanf" lo siguinete lo va a ignorar
	  //s=Okay "sscanf" ignore \n y \t
	 //Okay y guarde eso en mvpid
            sscanf(L, "%*s %ld", &mv);
            break;//sale para no segir iternado
        }
    }
    fclose(archistatus);
    return mv;
}


long Tmv() {
FILE *proc;
  char path[longitudmax];
  char L[256];
  long Tmf = -1;
  long TmSwap=-1;//la memoria del SSD que se usa cuando Tmf ya esta toda en uso 
  int listo=0;

  snprintf(path, longitudmax, "/proc/meminfo");
  proc = fopen(path, "r");
  if (proc == NULL) {
    perror("! open meminfo");
    return -1;
  }

  while (fgets(L, sizeof(L), proc)) {
    if (strncmp(L, "MemTotal:", 8) == 0) { //total de memoria en RAM fisico 
      sscanf(L, "%*s %ld", &Tmf);
      listo++;
    }
    if (strncmp(L, "SwapTotal:", 9) == 0) { //la memoria del SSD que se usa cuando Tmf ya esta toda en uso 
      sscanf(L, "%*s %ld", &TmSwap);
      listo++;
    }
    if(listo==2){
        break;
    }
  }
  fclose(proc);
  return (long)Tmf+(long)TmSwap;

}

long mf_pid(int pid) {
    char path[longitudmax];
    FILE *archistatus;
    char L[256];
    long mfpid = -1;

    snprintf(path, longitudmax, "/proc/%d/status", pid);

    archistatus = fopen(path, "r");
    if (archistatus == NULL) {
        perror("! open status");
        return -1;
    }

    while (fgets(L, sizeof(L), archistatus)) {
        if (strncmp(L, "VmRSS:", 6) == 0) {//esa parte es Mem fisica
            sscanf(L, "%*s %ld", &mfpid);
            break;
        }
    }
    fclose(archistatus);
    return mfpid;
}


long Tmf() {
  FILE *proc;
  char path[longitudmax];
  char L[256];
  long Tmf = -1;

  snprintf(path, longitudmax, "/proc/meminfo");
  proc = fopen(path, "r");
  if (proc == NULL) {
    perror("! open meminfo");
    return -1;
  }

  while (fgets(L, sizeof(L), proc)) {
    if (strncmp(L, "MemTotal:", 8) == 0) { //total de memoria en RAM fisico 
      sscanf(L, "%*s %ld", &Tmf);
      break;
    }
  }

  fclose(proc);
  return Tmf;
}
//Todos los metodos comparten muchas similitudes en temas de fromato, solo se lee la info de archivos diferentes

//Todos los metodos comparten muchas similitudes en temas de fromato, solo se lee la info de archivos diferentes

int main(int argc, char *argv[]) {
if (argc < 2) {
        printf("Falta la eleccion de una valida opcion: %s <-r o -v>\n", argv[0]);
        return 1;
}


    //memoria virtual----------------------------------------------------------------------------------------------------------------------------------------
    FILE *proc_file;///para leer 1, y solo 1 archivo de proce con un pid valido
    //por eso despues abro proc_dir, que se usa para ver si el PID se va a poder usar para jugar con un proc file, ya que si el
    //pid no es valido no se puede
    char path[longitudmax];
    int pid;
    long TOTALmv=-1;
    long mvpid;
    char nom[longitudmax];
    DIR *proc_dir = NULL;// este dir solo se usa para filtrar los PID que existen en proc los que estan corriendo
    struct dirent *entry; //se usa para reprecentar el dir

if(strcmp(argv[1], "-v") == 0){//memoria virtual----------------------------------------------------------------------------------------------------------------------------------------
    if(argc < 3){
        printf("en ./memoria falto <4444> pero ya que esta presente <-v> asumo que lo que quieres es memoria virtual");
    }
    else if(strcmp(argv[2], "4444") != 0){
        printf("en ./memoria falto <4444> pero ya que esta presente <-v> asumo que lo que quieres es memoria virtual");
    }
    TOTALmv=Tmv();
    printf("\n\n\n");
    printf("Memoria Virtual");
    printf("[Total de memoria virtual: %ld KB]\n", TOTALmv);
    printf("------------------------------------------------------------------\n\n");
    printf("PID\t Nombre\t \t Porcentaje de Utilización de Memoria Virtual\n\n");
    printf("------------------------------------------------------------------\n\n");
    ////////////////////////////////////////ABRE////////////////////////
    proc_dir = opendir("/proc");// este dir solo se usa para filtrar los PID que existen en proc los que estan corriendo
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
                mvpid = mv_pid(pid);
                    if (mvpid != -1) {
                        //comm: para sacar el nombre del proceso
                        snprintf(path, longitudmax, "/proc/%d/comm", pid);
                        proc_file = fopen(path, "r");////////////////////////////////////////ABRE////////////////////////
                        if (proc_file != NULL) {
                            fgets(nom, sizeof(nom), proc_file);//solo para sacar 1 valor, que de echo es el NOMBRE y esta en la primera linea
                            fclose(proc_file);///////////////////////////////////////////////CIERA///////////////////////
                            // Calcular el porcentaje de utilización de memoria virtual
                            //porceU = porcentaje de Utilizacion 
                            double porceU = (double)mvpid / TOTALmv * 100.0;
                            // Imprimir la información del proceso
                            printf("--------------------------------------------------------------------------\n");
                            printf("PID:%d\t| Nombre:%s\t| Porcentaje:%.2f%%\n", pid, nom, porceU);
                            printf("--------------------------------------------------------------------------\n");
                            //porceU = porcentaje de Utilizacion 
                        }
                    }
            }
        }
    }
    closedir(proc_dir);///////////////////////////////////////////////CIERA///////////////////////
    return 0;

}//memoria virtual----------------------------------------------------------------------------------------------------------------------------------------
    
    //memoria fisica----------------------------------------------------------------------------------------------------------------------------------------

if(strcmp(argv[1], "-r") == 0){//memoria fisica----------------------------------------------------------------------------------------------------------------------------------------
    long TOTALmf=Tmf();
    long mfpid;
    printf("\n\n\n");
    printf("Memoria Fisica");
    printf("[Total de memoria fisica: %ld KB]\n", TOTALmf);
    printf("------------------------------------------------------------------\n\n");
    printf("PID\t Nombre\t \t Porcentaje de Utilización de Memoria Fisica\n\n");
    printf("------------------------------------------------------------------\n\n");

    proc_dir = opendir("/proc");////////////////////////////////////////ABRE////////////////////////

    
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
                mfpid = mf_pid(pid);
                    if (mfpid != -1) {
                        //comm: para sacar el nombre del proceso
                        snprintf(path, longitudmax, "/proc/%d/comm", pid);
                        proc_file = fopen(path, "r");////////////////////////////////////////ABRE////////////////////////
                        if (proc_file != NULL) {
                            fgets(nom, sizeof(nom), proc_file);
                            fclose(proc_file);///////////////////////////////////////////////CIERA///////////////////////
                            // Calcular el porcentaje de utilización de memoria fisica
                            double porceUf = (double)mfpid / TOTALmf * 100.0;
                            // Imprimir la información del proceso
                            printf("--------------------------------------------------------------------------\n");
                            printf("PID:%d\t| Nombre:%s\t| Porcentaje:%.2f%%\n", pid, nom, porceUf);
                            printf("--------------------------------------------------------------------------\n");
                        }
                    }
            }
        }
    }
    closedir(proc_dir);///////////////////////////////////////////////CIERA///////////////////////
    return 0;
}
else {
        printf("Esa opcion no es valida: %s\n", argv[1]);
        return 1;
}


    return 0;
}
