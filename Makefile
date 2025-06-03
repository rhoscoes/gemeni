# Makefile
CC = gcc
CFLAGS = -Wall -pthread -std=c11 # -Wall para todas las advertencias, -pthread para hilos, -std=c11 para C11

# Archivos fuente (C)
# Si sim_types.h es solo un archivo de cabecera con typedefs y defines,
# no debe estar aquí. Solo se incluye en otros .c.
SRCS = main.c \
       utils.c \
       zone_manager.c \
       traffic_manager.c \
       io_manager.c \
       time_manager.c \
       print_manager.c

# Archivos objeto (generados a partir de los fuentes)
OBJS = $(SRCS:.c=.o)

# Nombre del ejecutable
TARGET = simcaracas

# Reglas generales para Make
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Regla para compilar archivos .c en .o
# El indicador -c le dice a gcc que solo compile, no que enlace.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
