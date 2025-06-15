#include "hardware.h"
#include "data.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* tipo_to_string(TipoHardware tipo) {
    switch(tipo) {
        case COMPUTADOR: return "COMPUTADOR";
        case IMPRESSORA: return "IMPRESSORA";
        case SERVIDOR: return "SERVIDOR";
        case ROTEADOR: return "ROTEADOR";
        case SWITCH: return "SWITCH";
        default: return "OUTRO";
    }
}

TipoHardware string_to_tipo(const char* str) {
    if (strcmp(str, "COMPUTADOR") == 0) return COMPUTADOR;
    if (strcmp(str, "IMPRESSORA") == 0) return IMPRESSORA;
    if (strcmp(str, "SERVIDOR") == 0) return SERVIDOR;
    if (strcmp(str, "ROTEADOR") == 0) return ROTEADOR;
    if (strcmp(str, "SWITCH") == 0) return SWITCH;
    return OUTRO;
}

char* hardware_to_csv(const Hardware* hw) {
    char* dataCompraStr = data_to_string(&hw->dataCompra);
    char* ultimaManutencaoStr = data_to_string(&hw->ultimaManutencao);
    
    int size = snprintf(NULL, 0, "%d;%s;%s;%s;%s;%.2f;%d;%s;%d",
                        hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                        dataCompraStr, hw->valorCompra, hw->vidaUtilAnos,
                        ultimaManutencaoStr, hw->obsoleto ? 1 : 0);
    
    char* csv = malloc(size + 1);
    if (csv) {
        sprintf(csv, "%d;%s;%s;%s;%s;%.2f;%d;%s;%d",
                hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                dataCompraStr, hw->valorCompra, hw->vidaUtilAnos,
                ultimaManutencaoStr, hw->obsoleto ? 1 : 0);
    }
    
    free(dataCompraStr);
    free(ultimaManutencaoStr);
    return csv;
}

bool hardware_from_csv(const char* linha, Hardware* hw) {
    char linha_copy[1024];
    strncpy(linha_copy, linha, sizeof(linha_copy) - 1);
    linha_copy[sizeof(linha_copy) - 1] = '\0';

    char* current_ptr = linha_copy;
    char* token_start = linha_copy;
    int i = 0;
    
    char* temp_campos[9]; 

    while (*current_ptr != '\0' && i < 9) {
        if (*current_ptr == ';') {
            *current_ptr = '\0';
            temp_campos[i++] = token_start;
            token_start = current_ptr + 1;
        }
        current_ptr++;
    }
    if (i < 9) {
        temp_campos[i++] = token_start;
    }
    
    if (i != 9) {
        return false;
    }
    
    hw->id = atoi(temp_campos[0]);
    strncpy(hw->nome, temp_campos[1], sizeof(hw->nome) - 1);
    hw->nome[sizeof(hw->nome) - 1] = '\0';
    
    strncpy(hw->fabricante, temp_campos[2], sizeof(hw->fabricante) - 1);
    hw->fabricante[sizeof(hw->fabricante) - 1] = '\0';

    hw->tipo = string_to_tipo(temp_campos[3]);
    
    if (!data_from_string(temp_campos[4], &hw->dataCompra)) {
        return false;
    }
    
    hw->valorCompra = atof(temp_campos[5]);
    hw->vidaUtilAnos = atoi(temp_campos[6]);
    
    if (!data_from_string(temp_campos[7], &hw->ultimaManutencao)) {
        return false;
    }
    
    hw->obsoleto = (strcmp(temp_campos[8], "1") == 0 || strcmp(temp_campos[8], "1\n") == 0); 
    
    return true;
}

char* hardware_to_string(const Hardware* hw) {
    char* dataCompraStr = data_to_string(&hw->dataCompra);
    char* ultimaManutencaoStr = data_to_string(&hw->ultimaManutencao);
    
    int size = snprintf(NULL, 0, "ID: %d | %s (%s) | Tipo: %s | Compra: %s | Última manutenção: %s | Valor: R$%.2f | Vida útil: %d anos | %s",
                        hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                        dataCompraStr, ultimaManutencaoStr, hw->valorCompra,
                        hw->vidaUtilAnos, hw->obsoleto ? "OBSOLETO" : "Ativo");
    
    char* str = malloc(size + 1);
    if (str) {
        sprintf(str, "ID: %d | %s (%s) | Tipo: %s | Compra: %s | Última manutenção: %s | Valor: R$%.2f | Vida útil: %d anos | %s",
                hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                dataCompraStr, ultimaManutencaoStr, hw->valorCompra,
                hw->vidaUtilAnos, hw->obsoleto ? "OBSOLETO" : "Ativo");
    }
    
    free(dataCompraStr);
    free(ultimaManutencaoStr);
    return str;
}