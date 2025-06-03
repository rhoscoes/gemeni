// zone_manager.c
#include "zone_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // Para errno
#include <sys/select.h> // Para select (aunque aquí se usará read directo)

// Función que ejecutará cada hilo de zona
void* zone_handler_thread(void* arg) {
    ZoneThreadArgs* args = (ZoneThreadArgs*)arg;
    printf("[ZONA HILO %s %d]: Iniciado. Manejando zona %s.\n", args->codigo_zona, (int)pthread_self(), args->codigo_zona);
    // Aquí eventualmente irá la lógica BFS, actualización de puntos, etc.

    // Por ahora, solo simular que hace algo y luego termina
    sleep(1);
    printf("[ZONA HILO %s %d]: Terminando.\n", args->codigo_zona, (int)pthread_self());
    free(args); // Liberar la memoria de los argumentos
    pthread_exit(NULL);
}

// Función principal del proceso ZONAS
void run_zonas_process(int read_pipe_fd, int write_pipe_fd) {
    printf("[ZONAS %d]: Iniciado.\n", getpid());

    char buffer[MSG_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(read_pipe_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("[ZONAS %d]: Recibido de IO: '%s'\n", getpid(), buffer);
        if (strcmp(buffer, CMD_SHUTDOWN) == 0) {
            break; // Si recibe SHUTDOWN, sale del bucle
        } else if (strncmp(buffer, CMD_ZONAS_ADD, strlen(CMD_ZONAS_ADD)) == 0) {
            char zone_code[4];
            if (sscanf(buffer, "%*s %3s", zone_code) == 1) {
                ZoneThreadArgs* args = (ZoneThreadArgs*)malloc(sizeof(ZoneThreadArgs));
                if (args == NULL) {
                    perror("[ZONAS]: Error al asignar memoria para ZoneThreadArgs");
                    continue;
                }
                strncpy(args->codigo_zona, zone_code, 3);
                args->codigo_zona[3] = '\0';

                pthread_t new_zone_tid;
                if (pthread_create(&new_zone_tid, NULL, zone_handler_thread, (void*)args) != 0) {
                    perror("[ZONAS]: Error al crear hilo de zona");
                    free(args);
                } else {
                    pthread_detach(new_zone_tid);
                    printf("[ZONAS %d]: Hilo para zona %s creado (TID: %lu).\n", getpid(), zone_code, (unsigned long)new_zone_tid);
                }
            } else {
                printf("[ZONAS %d]: Formato de comando ADD_ZONE inválido: '%s'\n", getpid(), buffer);
            }
        } else if (strcmp(buffer, CMD_TIME_TICK) == 0 ||
                   strcmp(buffer, CMD_END_MORNING) == 0 ||
                   strcmp(buffer, CMD_END_AFTERNOON) == 0) {
            printf("[ZONAS %d]: Recibido mensaje de tiempo: '%s'\n", getpid(), buffer);
            // Aquí iría la lógica para que las zonas (o sus hilos) reaccionen a los ticks de tiempo
        }
        // Responder al Main a través de IO (ACK genérico por ahora)
        const char* response = "ACK_ZONAS";
        write(write_pipe_fd, response, strlen(response) + 1);
    }
    printf("[ZONAS %d]: Terminando.\n", getpid());
    close(read_pipe_fd);
    close(write_pipe_fd);
    exit(EXIT_SUCCESS);
}
