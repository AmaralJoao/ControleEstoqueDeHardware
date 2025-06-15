#ifndef UTILS_H
#define UTILS_H

#include "data.h"
#include "hardware.h"

bool compare_data_compra(const Hardware* a, const Hardware* b);
bool compare_data_manutencao(const Hardware* a, const Hardware* b);
TipoHardware selecionar_tipo();
bool ler_data(const char* mensagem, Data* data);
void limpar_buffer_entrada();

#endif // UTILS_H