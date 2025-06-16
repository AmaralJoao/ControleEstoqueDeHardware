#include "menu.h"
#include "sistemaInventario.h"
#include "utils.h"
#include "data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void menu_principal(Repository* repo) {
    Cronometro crono_total;
    cronometro_iniciar(&crono_total);
    
    SistemaInventario sistema;
    sistema_init(&sistema, repo); 
    
    Data hoje = obter_data_atual();
    char* hojeStr = data_to_string(&hoje);
    
    printf("\n=== SISTEMA DE INVENTÁRIO DE HARDWARE ===\n");
    printf("Data atual do sistema: %s\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);

    int opcao;
    bool sair = false;
    
    while (!sair) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1 - Cadastrar novo hardware\n");
        printf("2 - Registrar manutenção\n");
        printf("3 - Listar todos os equipamentos\n");
        printf("4 - Listar equipamentos por tipo\n");
        printf("5 - Listar equipamentos ordenados por data de compra\n");
        printf("6 - Listar equipamentos ordenados por data de manutenção\n");
        printf("7 - Mostrar análise de depreciação\n");
        printf("8 - Identificar equipamentos obsoletos\n");
        printf("9 - Relatório de manutenção pendente\n");
        printf("0 - Sair\n");
        printf("Opção: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer_entrada();
            printf("Entrada inválida! Digite um número entre 0 e 9.\n");
            continue;
        }
        limpar_buffer_entrada();

        Cronometro crono_op;
        cronometro_iniciar(&crono_op);
        
        switch(opcao) {
            case 1: {
                char nome[100], fabricante[100];
                printf("\n--- CADASTRO DE HARDWARE ---\n");
                
                do {
                    printf("Nome do equipamento: ");
                    if (fgets(nome, sizeof(nome), stdin) == NULL) {
                        printf("Erro ao ler entrada.\n");
                        continue;
                    }
                    nome[strcspn(nome, "\n")] = '\0';
                } while (strlen(nome) == 0);
                
                do {
                    printf("Fabricante: ");
                    if (fgets(fabricante, sizeof(fabricante), stdin) == NULL) {
                        printf("Erro ao ler entrada.\n");
                        continue;
                    }
                    fabricante[strcspn(fabricante, "\n")] = '\0';
                } while (strlen(fabricante) == 0);
                
                TipoHardware tipo = selecionar_tipo();
                Data dataCompra;
                
                while (!ler_data("Data de compra (DD/MM/AAAA)", &dataCompra)) {
                    printf("Data inválida! Tente novamente.\n");
                }
                
                double valor = 0;
                do {
                    printf("Valor de compra (R$): ");
                    if (scanf("%lf", &valor) != 1 || valor <= 0) {
                        limpar_buffer_entrada();
                        printf("Valor inválido! Digite um valor positivo.\n");
                        continue;
                    }
                    limpar_buffer_entrada();
                    break;
                } while (true);
                
                int vidaUtil = 0;
                do {
                    printf("Vida útil em anos: ");
                    if (scanf("%d", &vidaUtil) != 1 || vidaUtil <= 0) {
                        limpar_buffer_entrada();
                        printf("Valor inválido! Digite um número positivo.\n");
                        continue;
                    }
                    limpar_buffer_entrada();
                    break;
                } while (true);
                
                if (!sistema_cadastrar_hardware(&sistema, nome, fabricante, tipo, &dataCompra, valor, vidaUtil)) {
                    printf("Erro ao cadastrar hardware!\n");
                }
                break;
            }
            
            case 2: {
                printf("\n--- REGISTRAR MANUTENÇÃO ---\n");
                int id = 0;
                
                do {
                    printf("ID do equipamento: ");
                    if (scanf("%d", &id) != 1 || id <= 0) {
                        limpar_buffer_entrada();
                        printf("ID inválido! Digite um número positivo.\n");
                        continue;
                    }
                    limpar_buffer_entrada();
                    break;
                } while (true);
                
                Data dataManutencao;
                while (!ler_data("Data da manutenção (DD/MM/AAAA)", &dataManutencao)) {
                    printf("Data inválida! Tente novamente.\n");
                }
                
                if (!sistema_registrar_manutencao(&sistema, id, &dataManutencao)) {
                    printf("Equipamento não encontrado!\n");
                } else {
                    printf("Manutenção registrada com sucesso!\n");
                }
                break;
            }
            
            case 3:
                sistema_listar_equipamentos(&sistema);
                break;
                
            case 4: {
                TipoHardware tipo = selecionar_tipo();
                sistema_listar_por_tipo(&sistema, tipo);
                break;
            }
            
            case 5:
                sistema_listar_por_data_compra(&sistema);
                break;
                
            case 6:
                sistema_listar_por_data_manutencao(&sistema);
                break;
                
            case 7:
                sistema_mostrar_analise_depreciacao(&sistema, &hoje);
                break;
                
            case 8:
                sistema_identificar_obsoletos(&sistema, &hoje);
                break;
                
            case 9: {
                printf("\n--- RELATÓRIO DE MANUTENÇÃO PENDENTE ---\n");
                int meses = 0;
                
                do {
                    printf("Informe o limite de meses sem manutenção: ");
                    if (scanf("%d", &meses) != 1 || meses <= 0) {
                        limpar_buffer_entrada();
                        printf("Valor inválido! Digite um número positivo.\n");
                        continue;
                    }
                    limpar_buffer_entrada();
                    break;
                } while (true);
                
                sistema_relatorio_manutencao_pendente(&sistema, &hoje, meses);
                break;
            }
            
            case 0:
                printf("\nSalvando dados e saindo...\n");
                sair = true;
                break;
                
            default:
                printf("Opção inválida! Digite um número entre 0 e 9.\n");
                break;
        }
        
        double tempo_op = cronometro_parar(&crono_op);
        cronometro_imprimir("Operação do menu", tempo_op);
    }
    
    sistema_destroy(&sistema);
    
    double tempo_total = cronometro_parar(&crono_total);
    cronometro_imprimir("Tempo total no menu", tempo_total);
}