#ifndef HARDWARE_H
#define HARDWARE_H

#include "data.h"
#include <stdbool.h>

typedef enum {
    COMPUTADOR,
    IMPRESSORA,
    SERVIDOR,
    ROTEADOR,
    SWITCH,
    OUTRO
} TipoHardware;

typedef struct {
    int id;
    char nome[100];
    char fabricante[100];
    TipoHardware tipo;
    Data dataCompra;
    double valorCompra;
    int vidaUtilAnos;
    Data ultimaManutencao;
    bool obsoleto;
} Hardware;

char* tipo_to_string(TipoHardware tipo);
TipoHardware string_to_tipo(const char* str);
char* hardware_to_csv(const Hardware* hw);
bool hardware_from_csv(const char* linha, Hardware* hw);
char* hardware_to_string(const Hardware* hw);

#endif // HARDWARE_H