#ifndef DATA_H
#define DATA_H

typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

Data obter_data_atual();
char* data_to_string(const Data* data);
bool data_from_string(const char* str, Data* data);
bool data_menor_que(const Data* a, const Data* b);

#endif 