// traffic_manager.h
#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include "sim_types.h"
#include <unistd.h>

// Prototipos de funciones
void run_trafico_process(int read_pipe_fd, int write_pipe_fd);

#endif // TRAFFIC_MANAGER_H
