// traffic_manager.c
#include "traffic_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función principal del proceso TRAFICO
void run_trafico_process(int read_pipe_fd, int write_pipe_fd) {
    printf("[TRAFICO %d]: Iniciado.\n", getpid());
    char buffer[MSG_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(read_pipe_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("[TRAFICO %d]: Recibido de IO: '%s'\n", getpid(), buffer);
        if (strcmp(buffer, CMD_SHUTDOWN) == 0) {
            break;
        } else if (strcmp(buffer, CMD_TIME_TICK) == 0 ||
                   strcmp(buffer, CMD_END_MORNING) == 0 ||
                   strcmp(buffer, CMD_END_AFTERNOON) == 0) {
            printf("[TRAFICO %d]: Recibido mensaje de tiempo: '%s'\n", getpid(), buffer);
            // Aquí iría la lógica para que Tráfico (o sus hilos) reaccionen a los ticks de tiempo
        }
        const char* response = "ACK_TRAFICO";
        write(write_pipe_fd, response, strlen(response) + 1);
    }
    printf("[TRAFICO %d]: Terminando.\n", getpid());
    close(read_pipe_fd);
    close(write_pipe_fd);
    exit(EXIT_SUCCESS);
}
