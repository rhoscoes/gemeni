// time_manager.c
#include "time_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Función principal del proceso TIEMPO
void run_tiempo_process(int read_pipe_fd, int write_pipe_fd) {
    printf("[TIEMPO %d]: Iniciado.\n", getpid());
    char buffer[MSG_SIZE];
    int day_ticks = 10; // Ejemplo: 10 ticks por día
    int morning_ticks = day_ticks / 2; // Ejemplo: 5 ticks de mañana
    int current_tick = 0;
    int running = 1;
    int morning_ended = 0;
    int day_ended = 0;

    fd_set read_fds_tiempo;
    int max_fd_tiempo = read_pipe_fd; // Solo un pipe de lectura
    struct timeval tv;

    while(running) {
        FD_ZERO(&read_fds_tiempo);
        FD_SET(read_pipe_fd, &read_fds_tiempo);

        tv.tv_sec = 1; // 1 segundo por cada tick
        tv.tv_usec = 0;

        int activity = select(max_fd_tiempo + 1, &read_fds_tiempo, NULL, NULL, &tv);

        if (activity < 0 && errno != EINTR) {
            perror("[TIEMPO]: Error en select");
            running = 0;
            break;
        }

        if (activity > 0 && FD_ISSET(read_pipe_fd, &read_fds_tiempo)) {
            ssize_t bytes_read = read(read_pipe_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("[TIEMPO %d]: Recibido de IO: '%s'\n", getpid(), buffer);
                if (strcmp(buffer, CMD_SHUTDOWN) == 0) {
                    running = 0;
                }
            } else if (bytes_read == 0) {
                printf("[TIEMPO %d]: IO cerró su tubería de escritura. Terminando.\n", getpid());
                running = 0;
            }
        }

        if (running && !day_ended) {
            current_tick++;
            char msg[50]; // Suficiente para TIME_TICK <numero>
            sprintf(msg, "%s %d", CMD_TIME_TICK, current_tick);
            write(write_pipe_fd, msg, strlen(msg) + 1);
            printf("[TIEMPO %d]: Enviado: '%s'\n", getpid(), msg);

            if (current_tick >= morning_ticks && !morning_ended) {
                printf("[TIEMPO %d]: Fin de la mañana (tick %d).\n", getpid(), current_tick);
                write(write_pipe_fd, CMD_END_MORNING, strlen(CMD_END_MORNING) + 1);
                morning_ended = 1;
            }

            if (current_tick >= day_ticks && !day_ended) {
                printf("[TIEMPO %d]: Fin del día (tick %d).\n", getpid(), current_tick);
                write(write_pipe_fd, CMD_END_AFTERNOON, strlen(CMD_END_AFTERNOON) + 1);
                day_ended = 1;
                // Si el día terminó, el proceso de Tiempo ya no necesita generar más ticks,
                // solo espera el SHUTDOWN del IO.
            }
        }
    }
    printf("[TIEMPO %d]: Terminando.\n", getpid());
    close(read_pipe_fd);
    close(write_pipe_fd);
    exit(EXIT_SUCCESS);
}
