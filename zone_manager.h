// zone_manager.h
#ifndef ZONE_MANAGER_H
#define ZONE_MANAGER_H

#include "sim_types.h" // Necesario para ZoneThreadArgs
#include <unistd.h>     // Para ssize_t, etc.

// Prototipos de funciones
void* zone_handler_thread(void* arg);
void run_zonas_process(int read_pipe_fd, int write_pipe_fd);

#endif // ZONE_MANAGER_H
