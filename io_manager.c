// io_manager.c
#include "io_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Función principal del proceso IO
void run_io_process(int main_read_pipe, int main_write_pipe,
                    int zonas_read_pipe, int zonas_write_pipe,
                    int trafico_read_pipe, int trafico_write_pipe,
                    int tiempo_read_pipe, int tiempo_write_pipe,
                    int impresion_read_pipe, int impresion_write_pipe) {
    printf("[IO %d]: Iniciado. Soy el Mensajero.\n", getpid());

    fd_set read_fds;
    int max_fd = 0;
    char buffer[MSG_SIZE];
    int running = 1;

    // Inicializar max_fd con el máximo de los descriptores de lectura
    if (main_read_pipe > max_fd) max_fd = main_read_pipe;
    if (zonas_read_pipe > max_fd) max_fd = zonas_read_pipe;
    if (trafico_read_pipe > max_fd) max_fd = trafico_read_pipe;
    if (tiempo_read_pipe > max_fd) max_fd = tiempo_read_pipe;
    if (impresion_read_pipe > max_fd) max_fd = impresion_read_pipe;


    while (running) {
        FD_ZERO(&read_fds);

        FD_SET(main_read_pipe, &read_fds);
        FD_SET(zonas_read_pipe, &read_fds);
        FD_SET(trafico_read_pipe, &read_fds);
        FD_SET(tiempo_read_pipe, &read_fds);
        FD_SET(impresion_read_pipe, &read_fds);

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("[IO]: Error en select");
            running = 0;
            break;
        }

        // --- Procesar datos de las tuberías ---

        // Mensaje del Main al IO
        if (FD_ISSET(main_read_pipe, &read_fds)) {
            ssize_t bytes_read = read(main_read_pipe, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) {
                printf("[IO]: Main cerró su tubería. Terminando.\n");
                running = 0;
                // No es necesario FD_CLR ni close aquí, el bucle terminará
                continue; // Saltar al inicio del bucle para la condición de 'running'
            }
            buffer[bytes_read] = '\0';
            printf("[IO %d]: Recibido del Main: '%s'\n", getpid(), buffer);

            if (strncmp(buffer, CMD_ZONAS_ADD, strlen(CMD_ZONAS_ADD)) == 0) {
                printf("[IO %d]: Reenviando '%s' a Zonas.\n", getpid(), buffer);
                write(zonas_write_pipe, buffer, bytes_read + 1);
            } else if (strcmp(buffer, CMD_SHUTDOWN) == 0) {
                printf("[IO %d]: Recibido SHUTDOWN del Main. Notificando a todos.\n", getpid());
                write(zonas_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                write(trafico_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                write(tiempo_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                write(impresion_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                running = 0;
            }
            // Más comandos del Main aquí
        }

        // Mensaje de Zonas al IO
        if (FD_ISSET(zonas_read_pipe, &read_fds)) {
            ssize_t bytes_read = read(zonas_read_pipe, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) { printf("[IO %d]: Zonas cerró su tubería. Removiendo de FD_SET.\n", getpid()); close(zonas_read_pipe); FD_CLR(zonas_read_pipe, &read_fds); continue; }
            buffer[bytes_read] = '\0';
            printf("[IO %d]: Recibido de Zonas: '%s'\n", getpid(), buffer);
            write(main_write_pipe, buffer, bytes_read + 1);
        }
        // Mensaje de Tráfico al IO
        if (FD_ISSET(trafico_read_pipe, &read_fds)) {
            ssize_t bytes_read = read(trafico_read_pipe, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) { printf("[IO %d]: Trafico cerró su tubería. Removiendo de FD_SET.\n", getpid()); close(trafico_read_pipe); FD_CLR(trafico_read_pipe, &read_fds); continue; }
            buffer[bytes_read] = '\0';
            printf("[IO %d]: Recibido de Tráfico: '%s'\n", getpid(), buffer);
            write(main_write_pipe, buffer, bytes_read + 1);
        }
        // Mensaje de Tiempo al IO
        if (FD_ISSET(tiempo_read_pipe, &read_fds)) {
            ssize_t bytes_read = read(tiempo_read_pipe, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) {
                printf("[IO %d]: Tiempo cerró su tubería. Removiendo de FD_SET.\n", getpid());
                close(tiempo_read_pipe); FD_CLR(tiempo_read_pipe, &read_fds);
                continue;
            }
            buffer[bytes_read] = '\0';
            printf("[IO %d]: Recibido de Tiempo: '%s'\n", getpid(), buffer);
            write(main_write_pipe, buffer, bytes_read + 1);

            if (strncmp(buffer, CMD_TIME_TICK, strlen(CMD_TIME_TICK)) == 0) {
                 printf("[IO %d]: Reenviando TIME_TICK a Zonas y Trafico.\n", getpid());
                 write(zonas_write_pipe, CMD_TIME_TICK, strlen(CMD_TIME_TICK) + 1);
                 write(trafico_write_pipe, CMD_TIME_TICK, strlen(CMD_TIME_TICK) + 1);
                 write(impresion_write_pipe, CMD_TIME_TICK, strlen(CMD_TIME_TICK) + 1); // También a Impresión
            } else if (strcmp(buffer, CMD_END_MORNING) == 0) {
                 printf("[IO %d]: Reenviando END_MORNING a Zonas y Trafico.\n", getpid());
                 write(zonas_write_pipe, CMD_END_MORNING, strlen(CMD_END_MORNING) + 1);
                 write(trafico_write_pipe, CMD_END_MORNING, strlen(CMD_END_MORNING) + 1);
                 write(impresion_write_pipe, CMD_END_MORNING, strlen(CMD_END_MORNING) + 1); // También a Impresión
            } else if (strcmp(buffer, CMD_END_AFTERNOON) == 0) {
                 printf("[IO %d]: Reenviando END_AFTERNOON a Zonas y Trafico.\n", getpid());
                 write(zonas_write_pipe, CMD_END_AFTERNOON, strlen(CMD_END_AFTERNOON) + 1);
                 write(trafico_write_pipe, CMD_END_AFTERNOON, strlen(CMD_END_AFTERNOON) + 1);
                 write(impresion_write_pipe, CMD_END_AFTERNOON, strlen(CMD_END_AFTERNOON) + 1); // También a Impresión
            } else if (strcmp(buffer, CMD_SHUTDOWN) == 0) {
                printf("[IO %d]: Recibido SHUTDOWN de Tiempo. Notificando a todos los demás.\n", getpid());
                write(zonas_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                write(trafico_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                write(impresion_write_pipe, CMD_SHUTDOWN, strlen(CMD_SHUTDOWN) + 1);
                running = 0;
            }
        }
        // Mensaje de Impresión al IO
        if (FD_ISSET(impresion_read_pipe, &read_fds)) {
            ssize_t bytes_read = read(impresion_read_pipe, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0) { printf("[IO %d]: Impresión cerró su tubería. Removiendo de FD_SET.\n", getpid()); close(impresion_read_pipe); FD_CLR(impresion_read_pipe, &read_fds); continue; }
            buffer[bytes_read] = '\0';
            printf("[IO %d]: Recibido de Impresión: '%s'\n", getpid(), buffer);
            write(main_write_pipe, buffer, bytes_read + 1);
        }
    } // Fin while(running) del proceso IO

    printf("[IO %d]: Terminando.\n", getpid());
    // Cierra todos los descriptores que el IO usó antes de salir
    close(main_read_pipe);
    close(main_write_pipe); // Aunque esta la usa el main para escribir, IO no debería tenerla abierta para escribir
    close(zonas_read_pipe);
    close(zonas_write_pipe);
    close(trafico_read_pipe);
    close(trafico_write_pipe);
    close(tiempo_read_pipe);
    close(tiempo_write_pipe);
    close(impresion_read_pipe);
    close(impresion_write_pipe);
    exit(EXIT_SUCCESS);
}
