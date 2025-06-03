// utils.c
#include "utils.h"
#include <stdlib.h> // Para malloc
#include <string.h> // Para strncpy, strcpy
#include <stdio.h>  // Para printf (para depuración, se puede quitar después)

void agregar_zona(Zona* grafo[], char codigo[], int nivel, int tipo) {
    for (int i = 0; i < MAX_ZONAS; i++) {
        if (grafo[i] == NULL) {
            Zona* nueva = (Zona*)malloc(sizeof(Zona));
            if (nueva == NULL) {
                perror("Error al asignar memoria para nueva Zona");
                return;
            }
            strncpy(nueva->codigo, codigo, 4);
            nueva->codigo[3] = '\0'; // Asegura terminación
            nueva->nivel = nivel;
            nueva->tipo = tipo;
            nueva->puntos = 0;
            nueva->empleados = 0;
            for (int j = 0; j < 4; j++) {
                nueva->conexiones[j] = NULL;
                nueva->capacidad_vias[j] = 0;
            }
            grafo[i] = nueva;
            printf("[UTILS]: Zona %s agregada al grafo en índice %d.\n", codigo, i);
            break;
        }
    }
}

void actualizar_congestion(Arteria* arteria) {
    if (arteria->capacidad_AB == 0 || arteria->capacidad_BA == 0) {
        strcpy(arteria->congestion_AB, ""); // Sin congestión si no hay capacidad
        strcpy(arteria->congestion_BA, "");
        return;
    }

    float ratio_AB = (float)arteria->vehiculos_AB / arteria->capacidad_AB;
    float ratio_BA = (float)arteria->vehiculos_BA / arteria->capacidad_BA;

    if (ratio_AB < 0.7)
        strcpy(arteria->congestion_AB, "");
    else if (ratio_AB < 1.0)
        strcpy(arteria->congestion_AB, "!");
    else if (ratio_AB < 1.3)
        strcpy(arteria->congestion_AB, "!!");
    else
        strcpy(arteria->congestion_AB, "!!!");

    if (ratio_BA < 0.7)
        strcpy(arteria->congestion_BA, "");
    else if (ratio_BA < 1.0)
        strcpy(arteria->congestion_BA, "!");
    else if (ratio_BA < 1.3)
        strcpy(arteria->congestion_BA, "!!");
    else
        strcpy(arteria->congestion_BA, "!!!");
    // printf("[UTILS]: Congestión actualizada para arteria entre %s y %s: AB='%s', BA='%s'\n",
    //        arteria->zonaA->codigo, arteria->zonaB->codigo,
    //        arteria->congestion_AB, arteria->congestion_BA);
}
