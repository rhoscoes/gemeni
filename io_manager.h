// io_manager.h
#ifndef IO_MANAGER_H
#define IO_MANAGER_H

#include "sim_types.h"
#include <unistd.h>
#include <sys/select.h> // Necesario para fd_set, select

// Prototipos de funciones
void run_io_process(int main_read_pipe, int main_write_pipe,
                    int zonas_read_pipe, int zonas_write_pipe,
                    int trafico_read_pipe, int trafico_write_pipe,
                    int tiempo_read_pipe, int tiempo_write_pipe,
                    int impresion_read_pipe, int impresion_write_pipe);

#endif // IO_MANAGER_H
