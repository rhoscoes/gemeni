// sim_types.h
#ifndef SIM_TYPES_H
#define SIM_TYPES_H

#include <pthread.h> // Necesario para pthread_t, etc. si ZoneThreadArgs lo usa

// --- Constantes generales ---
#define MAX_ZONAS 100
#define MSG_SIZE 256

// --- Comandos de Mensajes ---
#define CMD_ZONAS_ADD "ADD_ZONE"
#define CMD_TIME_TICK "TIME_TICK"
#define CMD_END_MORNING "END_MORNING"
#define CMD_END_AFTERNOON "END_AFTERNOON"
#define CMD_SHUTDOWN "SHUTDOWN" // Comando para terminar los procesos

// --- Estructuras de Datos del Grafo ---
typedef struct Zona {
    char codigo[4]; // +1 para '\0'
    int nivel;
    int tipo; // 0 = fuente, 1 = sumidero
    int puntos;
    int empleados; // o desempleados si es fuente
    struct Zona* conexiones[4]; // N, S, E, O (para conectar a otras zonas)
    int capacidad_vias[4]; // Capacidad de cada arteria que sale de esta zona
} Zona;

typedef struct Arteria {
    struct Zona* zonaA;
    struct Zona* zonaB;
    int capacidad_AB;      // Capacidad de A -> B
    int capacidad_BA;      // Capacidad de B -> A
    int vehiculos_AB;      // Vehículos en tránsito A -> B
    int vehiculos_BA;      // Vehículos en tránsito B -> A
    char congestion_AB[4]; // "!", "!!", "!!!"
    char congestion_BA[4];
} Arteria;

// --- Estructura para pasar argumentos a los hilos de zona ---
typedef struct {
    char codigo_zona[4];
    // Aquí puedes añadir más información si los hilos necesitan acceder a un grafo compartido, etc.
} ZoneThreadArgs;

#endif // SIM_TYPES_H
