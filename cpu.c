#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 128

int main() {

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

    return 0;
}

