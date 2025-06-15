#include "menu.h"
#include "sistemaInventario.h"
#include "utils.h"
#include "data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void menu_principal() {
    SistemaInventario sistema;
    sistema_init(&sistema);
    Data hoje = obter_data_atual();
    char* hojeStr = data_to_string(&hoje);
    
    printf("\n=== SISTEMA DE INVENTÁRIO DE HARDWARE ===\n");
    printf("Data atual do sistema: %s\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);

    int opcao;
    do {
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

        while (true) {
            if (scanf("%d", &opcao) != 1 || opcao < 0 || opcao > 9) {
                limpar_buffer_entrada();
                printf("Opção inválida! Tente novamente (0-9): ");
                continue;
            }
            break;
        }
        limpar_buffer_entrada();

        switch(opcao) {
            case 1: {
                char nome[100], fabricante[100];
                printf("\n--- CADASTRO DE HARDWARE ---\n");
                printf("Nome do equipamento: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strcspn(nome, "\n")] = '\0';
                
                printf("Fabricante: ");
                fgets(fabricante, sizeof(fabricante), stdin);
                fabricante[strcspn(fabricante, "\n")] = '\0';
                
                TipoHardware tipo = selecionar_tipo();
                Data dataCompra;
                ler_data("Data de compra", &dataCompra);
                
                double valor;
                printf("Valor de compra (R$): ");
                while (scanf("%lf", &valor) != 1 || valor <= 0) {
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um valor positivo: ");
                }
                limpar_buffer_entrada();
                
                int vidaUtil;
                printf("Vida útil em anos: ");
                while (scanf("%d", &vidaUtil) != 1 || vidaUtil <= 0) {
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                
                sistema_cadastrar_hardware(&sistema, nome, fabricante, tipo, &dataCompra, valor, vidaUtil);
                break;
            }
            case 2: {
                printf("\n--- REGISTRAR MANUTENÇÃO ---\n");
                int id;
                printf("ID do equipamento: ");
                while (scanf("%d", &id) != 1 || id <= 0) {
                    limpar_buffer_entrada();
                    printf("ID inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                Data dataManutencao;
                ler_data("Data da manutenção", &dataManutencao);
                if (sistema_registrar_manutencao(&sistema, id, &dataManutencao)) {
                    printf("Manutenção registrada com sucesso!\n");
                } else {
                    printf("Equipamento não encontrado!\n");
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
                int meses;
                printf("Informe o limite de meses sem manutenção: ");
                while (scanf("%d", &meses) != 1 || meses <= 0) {
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                sistema_relatorio_manutencao_pendente(&sistema, &hoje, meses);
                break;
            }
            case 0:
                printf("\nSalvando dados e saindo...\n");
                break;
        }
    } while (opcao != 0);
    
    sistema_destroy(&sistema);
}