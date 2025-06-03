// utils.h
#ifndef UTILS_H
#define UTILS_H

#include "sim_types.h" // Incluir sim_types.h para las definiciones de Zona y Arteria

// Prototipos de funciones utilitarias
void agregar_zona(Zona* grafo[], char codigo[], int nivel, int tipo);
void actualizar_congestion(Arteria* arteria);

#endif // UTILS_H
