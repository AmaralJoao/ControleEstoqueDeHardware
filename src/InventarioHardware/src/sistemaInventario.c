#include "sistemaInventario.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void sistema_init(SistemaInventario* sistema, Repository* repo) {
    linkedlist_init(&sistema->inventario);
    sistema->repositorio = repo;
    sistema->proximoId = 1;
    
    if (repo && repo->interface && repo->interface->carregar) {
        repo->interface->carregar(repo->implementacao, &sistema->inventario);
    }
    
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.id >= sistema->proximoId) {
            sistema->proximoId = current->data.id + 1;
        }
        current = current->next;
    }
}

void sistema_destroy(SistemaInventario* sistema) {
    if (sistema == NULL) return;

    // Salva os dados antes de destruir
    if (sistema->repositorio != NULL && 
        sistema->repositorio->interface != NULL && 
        sistema->repositorio->interface->salvar != NULL) {
        sistema->repositorio->interface->salvar(sistema->repositorio->implementacao, &sistema->inventario);
    }
    
    linkedlist_clear(&sistema->inventario);
    // Observação: O repositório não é destruído aqui - quem criou é responsável
}

bool sistema_cadastrar_hardware(SistemaInventario* sistema, const char* nome, const char* fabricante, 
                               TipoHardware tipo, const Data* dataCompra, double valorCompra, 
                               int vidaUtilAnos) {
    if (sistema == NULL || nome == NULL || fabricante == NULL || dataCompra == NULL) {
        return false;
    }

    // Validações dos parâmetros
    if (strlen(nome) == 0 || strlen(fabricante) == 0) {
        fprintf(stderr, "Nome e fabricante não podem estar vazios.\n");
        return false;
    }
    
    if (valorCompra <= 0 || vidaUtilAnos <= 0) {
        fprintf(stderr, "Valor e vida útil devem ser positivos.\n");
        return false;
    }

    // Cria o novo hardware
    Hardware hw;
    hw.id = sistema->proximoId++;
    strncpy(hw.nome, nome, sizeof(hw.nome) - 1);
    hw.nome[sizeof(hw.nome) - 1] = '\0';
    
    strncpy(hw.fabricante, fabricante, sizeof(hw.fabricante) - 1);
    hw.fabricante[sizeof(hw.fabricante) - 1] = '\0';

    hw.tipo = tipo;
    hw.dataCompra = *dataCompra;
    hw.valorCompra = valorCompra;
    hw.vidaUtilAnos = vidaUtilAnos;
    hw.ultimaManutencao = *dataCompra; // Data de compra como última manutenção inicial
    hw.obsoleto = false;

    // Adiciona na lista local
    linkedlist_push_back(&sistema->inventario, &hw);
    
    // Persiste no repositório
    if (sistema->repositorio != NULL && 
        sistema->repositorio->interface != NULL && 
        sistema->repositorio->interface->adicionar != NULL) {
        if (!sistema->repositorio->interface->adicionar(sistema->repositorio->implementacao, &hw)) {
            fprintf(stderr, "Erro ao salvar no repositório\n");
            // Remove da lista se não conseguiu salvar
            // (implementação simplificada - poderia ser mais robusta)
            linkedlist_clear(&sistema->inventario);
            if (sistema->repositorio->interface->carregar) {
                sistema->repositorio->interface->carregar(sistema->repositorio->implementacao, &sistema->inventario);
            }
            return false;
        }
    }
    
    printf("Hardware cadastrado com ID: %d\n", hw.id);
    return true;
}

bool sistema_registrar_manutencao(SistemaInventario* sistema, int id, const Data* dataManutencao) {
    if (sistema == NULL || dataManutencao == NULL) return false;

    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.id == id) {
            current->data.ultimaManutencao = *dataManutencao;
            
            // Atualiza no repositório se existir a função de atualização
            if (sistema->repositorio != NULL && 
                sistema->repositorio->interface != NULL && 
                sistema->repositorio->interface->atualizar != NULL) {
                return sistema->repositorio->interface->atualizar(sistema->repositorio->implementacao, &current->data);
            }
            return true;
        }
        current = current->next;
    }
    return false;
}

void sistema_listar_equipamentos(SistemaInventario* sistema) {
    if (sistema == NULL) return;

    printf("=== LISTA DE EQUIPAMENTOS (%d) ===\n", sistema->inventario.size);
    if (sistema->inventario.size == 0) {
        printf("Nenhum equipamento cadastrado.\n");
        return;
    }

    Node* current = sistema->inventario.head;
    while (current != NULL) {
        char* str = hardware_to_string(&current->data);
        if (str) {
            printf("%s\n", str);
            free(str);
        }
        current = current->next;
    }
}

void sistema_listar_por_tipo(SistemaInventario* sistema, TipoHardware tipo) {
    if (sistema == NULL) return;

    printf("=== EQUIPAMENTOS POR TIPO (%s) ===\n", tipo_to_string(tipo));
    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.tipo == tipo) {
            char* str = hardware_to_string(&current->data);
            if (str) {
                printf("%s\n", str);
                free(str);
            }
            contador++;
        }
        current = current->next;
    }
    printf("Total encontrado: %d equipamentos\n", contador);
}

void sistema_listar_por_data_compra(SistemaInventario* sistema) {
    if (sistema == NULL) return;

    LinkedList temp;
    linkedlist_init(&temp);
    
    // Copia os equipamentos para uma lista temporária
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        linkedlist_push_back(&temp, &current->data);
        current = current->next;
    }
    
    // Ordena pela data de compra
    linkedlist_insertion_sort(&temp, compare_data_compra);
    
    printf("=== EQUIPAMENTOS ORDENADOS POR DATA DE COMPRA ===\n");
    if (temp.size == 0) {
        printf("Nenhum equipamento para listar.\n");
    }
    
    current = temp.head;
    while (current != NULL) {
        char* str = hardware_to_string(&current->data);
        if (str) {
            printf("%s\n", str);
            free(str);
        }
        current = current->next;
    }
    
    linkedlist_clear(&temp);
}

void sistema_listar_por_data_manutencao(SistemaInventario* sistema) {
    if (sistema == NULL) return;

    LinkedList temp;
    linkedlist_init(&temp);
    
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        linkedlist_push_back(&temp, &current->data);
        current = current->next;
    }
    
    linkedlist_bubble_sort(&temp, compare_data_manutencao);
    
    printf("=== EQUIPAMENTOS ORDENADOS POR DATA DE MANUTENÇÃO ===\n");
    if (temp.size == 0) {
        printf("Nenhum equipamento para listar.\n");
    }
    
    current = temp.head;
    while (current != NULL) {
        char* str = hardware_to_string(&current->data);
        if (str) {
            printf("%s\n", str);
            free(str);
        }
        current = current->next;
    }
    
    linkedlist_clear(&temp);
}

double calcular_depreciacao(const Hardware* hw, const Data* hoje) {
    if (hw == NULL || hoje == NULL) return 0.0;

    int anos = hoje->ano - hw->dataCompra.ano;
    if (hoje->mes < hw->dataCompra.mes || (hoje->mes == hw->dataCompra.mes && hoje->dia < hw->dataCompra.dia)) {
        anos--;
    }
    
    if (anos <= 0) return 0.0;
    if (anos >= hw->vidaUtilAnos) return hw->valorCompra;
    
    return (hw->valorCompra / hw->vidaUtilAnos) * anos;
}

void sistema_mostrar_analise_depreciacao(SistemaInventario* sistema, const Data* hoje) {
    if (sistema == NULL || hoje == NULL) return;

    char* hojeStr = data_to_string(hoje);
    printf("=== ANÁLISE DE DEPRECIAÇÃO (Data base: %s) ===\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);

    if (sistema->inventario.size == 0) {
        printf("Nenhum equipamento para analisar.\n");
        return;
    }
    
    double total_original = 0, total_depreciado = 0;
    Node* current = sistema->inventario.head;
    
    while (current != NULL) {
        double depreciacao = calcular_depreciacao(&current->data, hoje);
        double valorAtual = current->data.valorCompra - depreciacao;
        
        printf("ID: %d | %s | Valor original: R$%.2f | Depreciação: R$%.2f | Valor atual: R$%.2f\n",
               current->data.id, current->data.nome, current->data.valorCompra,
               depreciacao, valorAtual);
        
        total_original += current->data.valorCompra;
        total_depreciado += depreciacao;
        current = current->next;
    }
    
    printf("----------------------------------------------------------------\n");
    printf("TOTAL | Valor original: R$%.2f | Depreciação total: R$%.2f | Valor atual total: R$%.2f\n",
           total_original, total_depreciado, (total_original - total_depreciado));
}

void sistema_atualizar_status_obsoleto(SistemaInventario* sistema, const Data* hoje) {
    if (sistema == NULL || hoje == NULL) return;

    Node* current = sistema->inventario.head;
    while (current != NULL) {
        int anos = hoje->ano - current->data.dataCompra.ano;
        if (hoje->mes < current->data.dataCompra.mes || 
            (hoje->mes == current->data.dataCompra.mes && hoje->dia < current->data.dataCompra.dia)) {
            anos--;
        }
        current->data.obsoleto = (anos >= current->data.vidaUtilAnos);
        current = current->next;
    }
}

void sistema_identificar_obsoletos(SistemaInventario* sistema, const Data* hoje) {
    if (sistema == NULL || hoje == NULL) return;

    sistema_atualizar_status_obsoleto(sistema, hoje);
    char* hojeStr = data_to_string(hoje);
    printf("=== EQUIPAMENTOS OBSOLETOS (Data base: %s) ===\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);
    
    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.obsoleto) {
            char* dataCompraStr = data_to_string(&current->data.dataCompra);
            printf("ID: %d | %s | Compra: %s | Vida útil: %d anos\n",
                   current->data.id, current->data.nome, 
                   dataCompraStr ? dataCompraStr : "ERRO", 
                   current->data.vidaUtilAnos);
            if (dataCompraStr) free(dataCompraStr);
            contador++;
        }
        current = current->next;
    }
    printf("Total de obsoletos: %d\n", contador);
}

void sistema_relatorio_manutencao_pendente(SistemaInventario* sistema, const Data* hoje, int mesesLimite) {
    if (sistema == NULL || hoje == NULL || mesesLimite <= 0) return;

    char* hojeStr = data_to_string(hoje);
    printf("=== EQUIPAMENTOS COM MANUTENÇÃO PENDENTE (> %d meses, Data base: %s) ===\n",
           mesesLimite, hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);
    
    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        int mesesDesdeManutencao = (hoje->ano - current->data.ultimaManutencao.ano) * 12 + 
                                   (hoje->mes - current->data.ultimaManutencao.mes);
        if (hoje->dia < current->data.ultimaManutencao.dia) {
            mesesDesdeManutencao--;
        }
        
        if (mesesDesdeManutencao >= mesesLimite) {
            char* ultimaManutencaoStr = data_to_string(&current->data.ultimaManutencao);
            printf("ID: %d | %s | Última manutenção: %s | Meses sem manutenção: %d\n",
                   current->data.id, current->data.nome, 
                   ultimaManutencaoStr ? ultimaManutencaoStr : "ERRO", 
                   mesesDesdeManutencao);
            if (ultimaManutencaoStr) free(ultimaManutencaoStr);
            contador++;
        }
        current = current->next;
    }
    printf("Total com manutenção pendente: %d\n", contador);
}