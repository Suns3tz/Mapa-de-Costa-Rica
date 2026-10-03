# Nombre del ejecutable final
TARGET = mapa

# Compiladores y banderas
CC = gcc
NASM = nasm

CFLAGS = -Wall -Wextra -std=c99 -g
NASMFLAGS = -f elf64
LIBS = -lGL -lGLU -lglut -lm

# Objetos que componen el proyecto
OBJS = map.o bresenham.o 

# Regla principal
all: $(TARGET)

# Enlazado final
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)
	@echo "--------------------------------------"
	@echo "¡Compilación exitosa! Ejecuta con: ./$(TARGET)"
	@echo "--------------------------------------"

# Compilación de archivos .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
