// time_manager.h
#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "sim_types.h"
#include <unistd.h>
#include <sys/select.h> // Necesario para fd_set, select, timeval

// Prototipos de funciones
void run_tiempo_process(int read_pipe_fd, int write_pipe_fd);

#endif // TIME_MANAGER_H
