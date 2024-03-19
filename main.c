#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 128

int main(int argc, char* argv[]) {
    if(argc <= 1){
	perror("parámetros");
	exit(EXIT_FAILURE);
    }
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];

    // Crear el pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Crear un proceso hijo
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0 && strcmp(argv[1], "cpu") == 0) { // Código del proceso hijo
        close(pipefd[0]); // Cerrar el descriptor de lectura del pipe en el proceso hijo

        // Redirigir la salida estándar al pipe
        dup2(pipefd[1], STDOUT_FILENO);

        // Ejecutar el programa para obtener el porcentaje de utilización de CPU
        execl("./cpu", "cpu", (char *)0);

        // Si el execl falla, imprimir un error y salir
        perror("Error al ejecutar el programa hijo");
        exit(EXIT_FAILURE);
    } else { // Código del proceso padre
        close(pipefd[1]); // Cerrar el descriptor de escritura del pipe en el proceso padre

        wait(NULL); // Esperar a que el proceso hijo termine

        // Leer el resultado del pipe
        read(pipefd[0], buffer, BUFFER_SIZE);
        printf("Porcentaje de utilización de CPU: %s%%\n", buffer);

        close(pipefd[0]); // Cerrar el descriptor de lectura del pipe en el proceso padre
    }

    return 0;
}
