// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h> // Necesario para el Main's select

#include "sim_types.h"
#include "utils.h"          // Para agregar_zona (aunque no lo usaremos en main directamente en esta versión simplificada)
#include "zone_manager.h"
#include "traffic_manager.h"
#include "io_manager.h"
#include "time_manager.h"
#include "print_manager.h"


int main(int argc, char *argv[]) {
    printf("Iniciando Sim Caracas - Preparando el mensajero...\n");

    // Zona* grafo[MAX_ZONAS] = {NULL}; // El grafo ahora se gestionará de forma compartida o en un módulo específico.
                                     // Por ahora, lo mantenemos como una variable local si no lo pasamos directamente a hijos

    // --- Declaración de todos los pares de tuberías ---
    int pipe_main_zonas[2];   int pipe_zonas_main[2];
    int pipe_main_trafico[2]; int pipe_trafico_main[2];
    int pipe_main_io[2];      int pipe_io_main[2];
    int pipe_main_tiempo[2];  int pipe_tiempo_main[2];
    int pipe_main_impresion[2]; int pipe_impresion_main[2];

    // --- Creación de todas las tuberías ---
    if (pipe(pipe_main_zonas) == -1 || pipe(pipe_zonas_main) == -1 ||
        pipe(pipe_main_trafico) == -1 || pipe(pipe_trafico_main) == -1 ||
        pipe(pipe_main_io) == -1 || pipe(pipe_io_main) == -1 ||
        pipe(pipe_main_tiempo) == -1 || pipe(pipe_tiempo_main) == -1 ||
        pipe(pipe_main_impresion) == -1 || pipe(pipe_impresion_main) == -1) {
        perror("Error creando una o más tuberías");
        exit(EXIT_FAILURE);
    }
    printf("Todas las tuberías creadas exitosamente.\n");

    pid_t pid_zonas, pid_trafico, pid_io, pid_tiempo, pid_impresion;

    // --- 1. Proceso de Manejo de Zonas ---
    pid_zonas = fork();
    if (pid_zonas == -1) { perror("Error al fork Zonas"); exit(EXIT_FAILURE); }
    else if (pid_zonas == 0) { // Código del Proceso Hijo (Zonas)
        close(pipe_main_zonas[1]);   // Zonas solo lee de Main a través de este pipe
        close(pipe_zonas_main[0]);   // Zonas solo escribe a Main a través de este pipe
        // Cierra los extremos de pipes no usados por Zonas
        close(pipe_main_trafico[0]); close(pipe_main_trafico[1]);
        close(pipe_trafico_main[0]); close(pipe_trafico_main[1]);
        close(pipe_main_io[0]);      close(pipe_main_io[1]);
        close(pipe_io_main[0]);      close(pipe_io_main[1]);
        close(pipe_main_tiempo[0]);  close(pipe_main_tiempo[1]);
        close(pipe_tiempo_main[0]);  close(pipe_tiempo_main[1]);
        close(pipe_main_impresion[0]); close(pipe_main_impresion[1]);
        close(pipe_impresion_main[0]); close(pipe_impresion_main[1]);

        run_zonas_process(pipe_main_zonas[0], pipe_zonas_main[1]);
        // run_zonas_process no retorna, hace exit()
    }

    // --- 2. Proceso de Manejo de Tráfico ---
    pid_trafico = fork();
    if (pid_trafico == -1) { perror("Error al fork Trafico"); exit(EXIT_FAILURE); }
    else if (pid_trafico == 0) { // Código del Proceso Hijo (Tráfico)
        close(pipe_main_trafico[1]);
        close(pipe_trafico_main[0]);
        // Cierra los extremos de pipes no usados por Tráfico
        close(pipe_main_zonas[0]);   close(pipe_main_zonas[1]);
        close(pipe_zonas_main[0]);   close(pipe_zonas_main[1]);
        close(pipe_main_io[0]);      close(pipe_main_io[1]);
        close(pipe_io_main[0]);      close(pipe_io_main[1]);
        close(pipe_main_tiempo[0]);  close(pipe_main_tiempo[1]);
        close(pipe_tiempo_main[0]);  close(pipe_tiempo_main[1]);
        close(pipe_main_impresion[0]); close(pipe_main_impresion[1]);
        close(pipe_impresion_main[0]); close(pipe_impresion_main[1]);

        run_trafico_process(pipe_main_trafico[0], pipe_trafico_main[1]);
    }

    // --- 3. Proceso de Manejo de I/O (Pipes) ---
    pid_io = fork();
    if (pid_io == -1) { perror("Error al fork IO"); exit(EXIT_FAILURE); }
    else if (pid_io == 0) { // Código del Proceso Hijo (I/O de Pipes)
        // Cierra los extremos de pipes que NO usará:
        close(pipe_main_io[1]);      // IO solo lee del Main
        close(pipe_io_main[0]);      // IO solo escribe al Main
        close(pipe_zonas_main[1]);   // IO solo lee de Zonas
        close(pipe_main_zonas[0]);   // IO solo escribe a Zonas
        close(pipe_trafico_main[1]); // IO solo lee de Trafico
        close(pipe_main_trafico[0]); // IO solo escribe a Trafico
        close(pipe_tiempo_main[1]);  // IO solo lee de Tiempo
        close(pipe_main_tiempo[0]);  // IO solo escribe a Tiempo
        close(pipe_impresion_main[1]); // IO solo lee de Impresion
        close(pipe_main_impresion[0]); // IO solo escribe a Impresion

        run_io_process(pipe_main_io[0], pipe_io_main[1],
                       pipe_zonas_main[0], pipe_main_zonas[1],
                       pipe_trafico_main[0], pipe_main_trafico[1],
                       pipe_tiempo_main[0], pipe_main_tiempo[1],
                       pipe_impresion_main[0], pipe_main_impresion[1]);
    }

    // --- 4. Proceso de Tiempo ---
    pid_tiempo = fork();
    if (pid_tiempo == -1) { perror("Error al fork Tiempo"); exit(EXIT_FAILURE); }
    else if (pid_tiempo == 0) { // Código del Proceso Hijo (Tiempo)
        close(pipe_main_tiempo[1]);
        close(pipe_tiempo_main[0]);
        // Cierra los extremos de pipes no usados por Tiempo
        close(pipe_main_zonas[0]);   close(pipe_main_zonas[1]);
        close(pipe_zonas_main[0]);   close(pipe_zonas_main[1]);
        close(pipe_main_trafico[0]); close(pipe_main_trafico[1]);
        close(pipe_trafico_main[0]); close(pipe_trafico_main[1]);
        close(pipe_main_io[0]);      close(pipe_main_io[1]);
        close(pipe_io_main[0]);      close(pipe_io_main[1]);
        close(pipe_main_impresion[0]); close(pipe_main_impresion[1]);
        close(pipe_impresion_main[0]); close(pipe_impresion_main[1]);

        run_tiempo_process(pipe_main_tiempo[0], pipe_tiempo_main[1]);
    }

    // --- 5. Proceso de Impresión por Consola ---
    pid_impresion = fork();
    if (pid_impresion == -1) { perror("Error al fork Impresión"); exit(EXIT_FAILURE); }
    else if (pid_impresion == 0) { // Código del Proceso Hijo (Impresión)
        close(pipe_main_impresion[1]);
        close(pipe_impresion_main[0]);
        // Cierra los extremos de pipes no usados por Impresión
        close(pipe_main_zonas[0]);   close(pipe_main_zonas[1]);
        close(pipe_zonas_main[0]);   close(pipe_zonas_main[1]);
        close(pipe_main_trafico[0]); close(pipe_main_trafico[1]);
        close(pipe_trafico_main[0]); close(pipe_trafico_main[1]);
        close(pipe_main_io[0]);      close(pipe_main_io[1]);
        close(pipe_io_main[0]);      close(pipe_io_main[1]);
        close(pipe_main_tiempo[0]);  close(pipe_main_tiempo[1]);
        close(pipe_tiempo_main[0]);  close(pipe_tiempo_main[1]);

        run_impresion_process(pipe_main_impresion[0], pipe_impresion_main[1]);
    }

    // --- Código del Proceso Principal (Padre) ---
    printf("[MAIN %d]: Proceso Principal iniciado.\n", getpid());
    // El Proceso Principal (Padre) USARÁ:
    // - pipe_main_io[1] para ESCRIBIR al IO
    // - pipe_io_main[0] para LEER del IO
    // Cierra todos los demás extremos de pipes en el Proceso Principal
    close(pipe_main_zonas[0]); close(pipe_main_zonas[1]);
    close(pipe_zonas_main[0]); close(pipe_zonas_main[1]);
    close(pipe_main_trafico[0]); close(pipe_main_trafico[1]);
    close(pipe_trafico_main[0]); close(pipe_trafico_main[1]);
    close(pipe_main_io[0]); // Cierra el lado de lectura del pipe del Main hacia IO
    close(pipe_io_main[1]); // Cierra el lado de escritura del pipe del IO hacia Main
    close(pipe_main_tiempo[0]); close(pipe_main_tiempo[1]);
    close(pipe_tiempo_main[0]); close(pipe_tiempo_main[1]);
    close(pipe_main_impresion[0]); close(pipe_main_impresion[1]);
    close(pipe_impresion_main[0]); close(pipe_impresion_main[1]);


    // --- Bucle principal del programa del Jefe (Main) ---
    printf("[MAIN %d]: Todos los procesos hijos han sido creados.\n", getpid());

    char command[MSG_SIZE];
    char response[MSG_SIZE];
    ssize_t bytes_read_main;

    // Simular envío de un comando "ADD_ZONE" con una zona específica
    strcpy(command, "ADD_ZONE CCS");
    printf("[MAIN %d]: Enviando comando '%s' a IO (para Zonas)...\n", getpid(), command);
    write(pipe_main_io[1], command, strlen(command) + 1);
    bytes_read_main = read(pipe_io_main[0], response, sizeof(response) - 1);
    if (bytes_read_main > 0) {
        response[bytes_read_main] = '\0';
        printf("[MAIN %d]: Recibido de IO (desde Zonas): '%s'\n", getpid(), response);
    } else {
        printf("[MAIN %d]: Error o EOF al leer respuesta de IO para ADD_ZONE.\n", getpid());
    }

    // Simular otro ADD_ZONE
    strcpy(command, "ADD_ZONE VZL");
    printf("[MAIN %d]: Enviando comando '%s' a IO (para Zonas)...\n", getpid(), command);
    write(pipe_main_io[1], command, strlen(command) + 1);
    bytes_read_main = read(pipe_io_main[0], response, sizeof(response) - 1);
    if (bytes_read_main > 0) {
        response[bytes_read_main] = '\0';
        printf("[MAIN %d]: Recibido de IO (desde Zonas): '%s'\n", getpid(), response);
    } else {
        printf("[MAIN %d]: Error o EOF al leer respuesta de IO para ADD_ZONE.\n", getpid());
    }


    printf("[MAIN %d]: Esperando la simulación de tiempo y los ACKs de los procesos...\n", getpid());
    int running_main_loop = 1;
    fd_set read_fds_main;
    int max_fd_main = pipe_io_main[0];

    while (running_main_loop) {
        FD_ZERO(&read_fds_main);
        FD_SET(pipe_io_main[0], &read_fds_main);
        struct timeval tv_main;
        tv_main.tv_sec = 2;
        tv_main.tv_usec = 0;

        int activity = select(max_fd_main + 1, &read_fds_main, NULL, NULL, &tv_main);

        if (activity < 0 && errno != EINTR) {
            perror("[MAIN]: Error en select");
            break;
        } else if (activity == 0) {
            printf("[MAIN %d]: Timeout esperando mensajes de IO. Posiblemente los procesos están inactivos o esperando entrada.\n", getpid());
            static int timeout_count = 0;
            timeout_count++;
            if (timeout_count >= 3) {
                 printf("[MAIN %d]: Demasiados timeouts. Forzando SHUTDOWN.\n", getpid());
                 strcpy(command, CMD_SHUTDOWN);
                 write(pipe_main_io[1], command, strlen(command) + 1);
                 running_main_loop = 0;
            }
            continue;
        }

        if (FD_ISSET(pipe_io_main[0], &read_fds_main)) {
            bytes_read_main = read(pipe_io_main[0], response, sizeof(response) - 1);
            if (bytes_read_main > 0) {
                response[bytes_read_main] = '\0';
                printf("[MAIN %d]: Recibido de IO: '%s'\n", getpid(), response);
                if (strcmp(response, CMD_SHUTDOWN) == 0) {
                    running_main_loop = 0;
                }
                if (strcmp(response, CMD_END_AFTERNOON) == 0) {
                    printf("[MAIN %d]: Día simulado terminado según el proceso de Tiempo.\n", getpid());
                    strcpy(command, CMD_SHUTDOWN);
                    write(pipe_main_io[1], command, strlen(command) + 1);
                    running_main_loop = 0;
                }
            } else {
                printf("[MAIN %d]: IO cerró su tubería de escritura. Terminando el bucle principal.\n", getpid());
                running_main_loop = 0;
            }
        }
    }


    printf("[MAIN %d]: Enviando comando '%s' a IO para apagar procesos (si no se hizo ya).\n", getpid(), CMD_SHUTDOWN);
    strcpy(command, CMD_SHUTDOWN);
    write(pipe_main_io[1], command, strlen(command) + 1);

    // Esperar a que los hijos terminen
    waitpid(pid_zonas, NULL, 0);
    waitpid(pid_trafico, NULL, 0);
    waitpid(pid_io, NULL, 0);
    waitpid(pid_tiempo, NULL, 0);
    waitpid(pid_impresion, NULL, 0);

    printf("[MAIN %d]: Todos los procesos hijos han terminado.\n", getpid());

    // NOTA: El grafo (Zona* grafo[MAX_ZONAS] = {NULL};) no está en main.c en esta versión modular.
    // Si necesitas liberar memoria del grafo, deberá hacerse donde se declare y gestione,
    // que idealmente sería en memoria compartida y manejada por un módulo específico del grafo.
    // Por ahora, eliminé el bucle de liberación del grafo del main.c ya que no es su responsabilidad directa.

    printf("Simulación terminada.\n");
    return 0;
}
