// print_manager.h
#ifndef PRINT_MANAGER_H
#define PRINT_MANAGER_H

#include "sim_types.h"
#include <unistd.h>

// Prototipos de funciones
void run_impresion_process(int read_pipe_fd, int write_pipe_fd);

#endif // PRINT_MANAGER_H
