#ifndef UTILS_H
#define UTILS_H

#include "data.h"
#include "hardware.h"
#include <stdbool.h>
#include <time.h>

typedef struct {
    clock_t inicio;
    clock_t fim;
} Cronometro;

bool compare_data_compra(const Hardware* a, const Hardware* b);
bool compare_data_manutencao(const Hardware* a, const Hardware* b);
TipoHardware selecionar_tipo();
bool ler_data(const char* mensagem, Data* data);
void limpar_buffer_entrada();
void cronometro_iniciar(Cronometro* cronometro);
double cronometro_parar(Cronometro* cronometro);
void cronometro_imprimir(const char* operacao, double tempo);

#endif // UTILS_H