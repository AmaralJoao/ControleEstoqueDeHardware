#include "data.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

Data obter_data_atual() {
    time_t agora = time(NULL);
    struct tm* tempo_local = localtime(&agora);
    
    Data atual;
    atual.dia = tempo_local->tm_mday;
    atual.mes = tempo_local->tm_mon + 1;
    atual.ano = tempo_local->tm_year + 1900;
    
    return atual;
}

char* data_to_string(const Data* data) {
    char* str = malloc(11); 
    if (str) {
        sprintf(str, "%02d/%02d/%04d", data->dia, data->mes, data->ano);
    }
    return str;
}

bool data_from_string(const char* str, Data* data) {
    char sep1, sep2;
    return sscanf(str, "%d%c%d%c%d", &data->dia, &sep1, &data->mes, &sep2, &data->ano) == 5 && sep1 == '/' && sep2 == '/';
}

bool data_menor_que(const Data* a, const Data* b) {
    if (a->ano != b->ano) return a->ano < b->ano;
    if (a->mes != b->mes) return a->mes < b->mes;
    return a->dia < b->dia;
}