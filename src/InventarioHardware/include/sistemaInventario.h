#ifndef SISTEMA_INVENTARIO_H
#define SISTEMA_INVENTARIO_H

#include "linkedList.h"
#include "repository.h"
#include <stdbool.h>

typedef struct {
    LinkedList inventario;
    Repository* repositorio; 
    int proximoId;
} SistemaInventario;

void sistema_init(SistemaInventario* sistema, Repository* repo); 
void sistema_destroy(SistemaInventario* sistema);
bool sistema_cadastrar_hardware(SistemaInventario* sistema, const char* nome, const char* fabricante, 
                               TipoHardware tipo, const Data* dataCompra, double valorCompra, 
                               int vidaUtilAnos);
bool sistema_registrar_manutencao(SistemaInventario* sistema, int id, const Data* dataManutencao);
void sistema_listar_equipamentos(SistemaInventario* sistema);
void sistema_listar_por_tipo(SistemaInventario* sistema, TipoHardware tipo);
void sistema_listar_por_data_compra(SistemaInventario* sistema);
void sistema_listar_por_data_manutencao(SistemaInventario* sistema);
double calcular_depreciacao(const Hardware* hw, const Data* hoje);
void sistema_mostrar_analise_depreciacao(SistemaInventario* sistema, const Data* hoje);
void sistema_atualizar_status_obsoleto(SistemaInventario* sistema, const Data* hoje);
void sistema_identificar_obsoletos(SistemaInventario* sistema, const Data* hoje);
void sistema_relatorio_manutencao_pendente(SistemaInventario* sistema, const Data* hoje, int mesesLimite);

#endif 