#include "utils.h"
#include "hardware.h"
#include "data.h"
#include <stdio.h>
#include <ctype.h>

bool compare_data_compra(const Hardware* a, const Hardware* b) {
    return data_menor_que(&a->dataCompra, &b->dataCompra);
}

bool compare_data_manutencao(const Hardware* a, const Hardware* b) {
    return data_menor_que(&a->ultimaManutencao, &b->ultimaManutencao);
}

TipoHardware selecionar_tipo() {
    printf("\nSelecione o tipo de hardware:\n");
    printf("1 - Computador\n2 - Impressora\n3 - Servidor\n");
    printf("4 - Roteador\n5 - Switch\n6 - Outro\n");
    printf("Opção: ");
    
    int op;
    while (true) {
        if (scanf("%d", &op) != 1 || op < 1 || op > 6) {
            limpar_buffer_entrada();
            printf("Opção inválida! Tente novamente (1-6): ");
            continue;
        }
        break;
    }
    limpar_buffer_entrada();

    switch(op) {
        case 1: return COMPUTADOR;
        case 2: return IMPRESSORA;
        case 3: return SERVIDOR;
        case 4: return ROTEADOR;
        case 5: return SWITCH;
        default: return OUTRO;
    }
}

bool ler_data(const char* mensagem, Data* data) {
    printf("%s (DD/MM/AAAA): ", mensagem);
    char sep1, sep2;
    while (true) {
        if (scanf("%d%c%d%c%d", &data->dia, &sep1, &data->mes, &sep2, &data->ano) != 5 || sep1 != '/' || sep2 != '/' || 
            data->dia < 1 || data->dia > 31 || data->mes < 1 || data->mes > 12 || data->ano < 1900) {
            limpar_buffer_entrada();
            printf("Data inválida! Use o formato DD/MM/AAAA: ");
            continue;
        }
        break;
    }
    limpar_buffer_entrada();
    return true;
}

void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}