.PHONY: all clean

# Compilador
CC = gcc

# Directorio para los ejecutables
BIN_DIR = /usr/local/bin

# Archivos fuente
SRCS = main.c disk.c cpu.c memoria.c


# Nombres de los programas finales
TARGETS = main disk cpu memoria
#cpu ram 

# Opciones de compilación
CFLAGS = -Wall

# Regla all para construir todos los targets
all: $(TARGETS)

# Reglas para construir los ejecutables
main: main.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/main main.c

disk: disk.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/disk disk.c

cpu: cpu.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/cpu cpu.c

memoria: memoria.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/memoria memoria.c

# Regla para limpiar los archivos compilados
clean:
	rm -f $(BIN_DIR)/$(TARGETS)
