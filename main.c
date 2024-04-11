#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
    if(argc <= 1){
	    fprintf(stderr, "Error de parámetros: se requiere al menos un argumento.\n");
        exit(EXIT_FAILURE);
        return 1;
    }
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Crear el pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0 ) { // Código del proceso hijo
        
        close(pipefd[0]); // Cerrar el descriptor de lectura del pipe en el proceso hijo

        
        dup2(pipefd[1], STDOUT_FILENO); // Redirigir la salida estándar al pipe
        if(strcmp(argv[1], "cpu") == 0){
        

        // Ejecutar el programa para obtener el porcentaje de utilización de CPU
        execl("./cpu", "cpu", argv[2], NULL);

        // Si el execl falla, imprimir un error y salir
        perror("Error al ejecutar el programa hijo");
        exit(EXIT_FAILURE);

        }else if(strcmp(argv[1], "disk") == 0){
            if(argc <= 2){
	            fprintf(stderr, "Error de parámetros: se requieren más argumentos\n");
                exit(EXIT_FAILURE);
                return 1;
            }
            execl("./disk", "disk", argv[2], NULL);
            // Si el execl falla, imprimir un error y salir
            perror("Error al ejecutar el programa hijo");
            exit(EXIT_FAILURE);
        }
        
    } else { // Código del proceso padre
        close(pipefd[1]); // Cerrar el descriptor de escritura del pipe en el proceso padre

        wait(NULL); // Esperar a que el proceso hijo termine

        // Leer el resultado del pipe
        read(pipefd[0], buffer, BUFFER_SIZE);
        printf("\n%s\n", buffer);

        close(pipefd[0]); // Cerrar el descriptor de lectura del pipe en el proceso padre
    }

    return 0;
}
