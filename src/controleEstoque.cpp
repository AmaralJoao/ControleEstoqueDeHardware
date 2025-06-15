#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>
#include <math.h>

// Enum para tipos de hardware (computador, impressora, etc)
typedef enum {
    COMPUTADOR,
    IMPRESSORA,
    SERVIDOR,
    ROTEADOR,
    SWITCH,
    OUTRO
} TipoHardware;

// Estrutura para representar uma data com dia, mês e ano
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

// Estrutura que representa um equipamento de hardware
// NOME E FABRICANTE AGORA SÃO ARRAYS DE TAMANHO FIXO PARA SIMPLIFICAR O GERENCIAMENTO DE MEMÓRIA
typedef struct {
    int id;
    char nome[100];         // Buffer de 100 caracteres para o nome
    char fabricante[100];   // Buffer de 100 caracteres para o fabricante
    TipoHardware tipo;
    Data dataCompra;
    double valorCompra;
    int vidaUtilAnos;
    Data ultimaManutencao;
    bool obsoleto;
} Hardware;

// Nó para lista encadeada (armazena um Hardware e ponteiro para o próximo)
typedef struct Node {
    Hardware data;
    struct Node* next;
} Node;

// Estrutura para lista encadeada
typedef struct {
    Node* head;
    Node* tail;
    int size;
} LinkedList;

// Sistema principal de inventário
typedef struct {
    LinkedList inventario;
    int proximoId;
    const char* arquivoDados;
} SistemaInventario;

// Protótipos de funções
Data obter_data_atual();
char* data_to_string(const Data* data);
bool data_from_string(const char* str, Data* data);
bool data_menor_que(const Data* a, const Data* b);
char* tipo_to_string(TipoHardware tipo);
TipoHardware string_to_tipo(const char* str);
char* hardware_to_csv(const Hardware* hw);
bool hardware_from_csv(const char* linha, Hardware* hw);
char* hardware_to_string(const Hardware* hw);
void linkedlist_init(LinkedList* list);
void linkedlist_clear(LinkedList* list);
void linkedlist_push_back(LinkedList* list, const Hardware* hw);
Node* linkedlist_get_head(const LinkedList* list);
int linkedlist_get_size(const LinkedList* list);

// Funções de comparação para ordenação
bool compare_data_compra(const Hardware* a, const Hardware* b);
bool compare_data_manutencao(const Hardware* a, const Hardware* b);

void linkedlist_bubble_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*));
void linkedlist_insertion_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*));
void sistema_init(SistemaInventario* sistema);
void sistema_destroy(SistemaInventario* sistema);
void sistema_carregar_dados(SistemaInventario* sistema);
void sistema_salvar_dados(SistemaInventario* sistema);
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
TipoHardware selecionar_tipo();
bool ler_data(const char* mensagem, Data* data);
void menu_principal();
void limpar_buffer_entrada();

// Implementação das funções

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
    // Aloca memória para a string da data (DD/MM/YYYY + \0)
    char* str = malloc(11); 
    if (str) {
        sprintf(str, "%02d/%02d/%04d", data->dia, data->mes, data->ano);
    }
    return str;
}

bool data_from_string(const char* str, Data* data) {
    char sep1, sep2;
    // Tenta ler a data no formato DD/MM/AAAA. Retorna true se 5 itens foram lidos e os separadores são '/'.
    return sscanf(str, "%d%c%d%c%d", &data->dia, &sep1, &data->mes, &sep2, &data->ano) == 5 && sep1 == '/' && sep2 == '/';
}

bool data_menor_que(const Data* a, const Data* b) {
    if (a->ano != b->ano) return a->ano < b->ano;
    if (a->mes != b->mes) return a->mes < b->mes;
    return a->dia < b->dia;
}

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
    
    // Calcula o tamanho necessário do buffer
    // Usa snprintf com NULL e 0 para obter o tamanho da string formatada
    int size = snprintf(NULL, 0, "%d;%s;%s;%s;%s;%.2f;%d;%s;%d",
                        hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                        dataCompraStr, hw->valorCompra, hw->vidaUtilAnos,
                        ultimaManutencaoStr, hw->obsoleto ? 1 : 0);
    
    char* csv = malloc(size + 1); // Aloca o buffer com o tamanho calculado + 1 para o terminador null
    if (csv) {
        // Formata a string CSV
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
    char linha_copy[1024]; // Buffer para a cópia da linha
    // Garante que a linha não exceda o tamanho do buffer
    strncpy(linha_copy, linha, sizeof(linha_copy) - 1);
    linha_copy[sizeof(linha_copy) - 1] = '\0'; // Garante terminação nula

    char* current_ptr = linha_copy;
    char* token_start = linha_copy;
    int i = 0;
    
    // Arrays temporários para armazenar ponteiros para os campos
    char* temp_campos[9]; 

    while (*current_ptr != '\0' && i < 9) {
        if (*current_ptr == ';') {
            *current_ptr = '\0'; // Termina o token atual
            temp_campos[i++] = token_start;
            token_start = current_ptr + 1; // Inicia o próximo token após o delimitador
        }
        current_ptr++;
    }
    // Adiciona o último token (ou o único token se não houver delimitadores)
    if (i < 9) {
        temp_campos[i++] = token_start;
    }
    
    if (i != 9) {
        // fprintf(stderr, "Erro ao parsear linha CSV: número incorreto de campos (%d em vez de 9) - %s\n", i, linha);
        return false;
    }
    
    hw->id = atoi(temp_campos[0]);
    // Copia o nome e fabricante para os arrays fixos
    strncpy(hw->nome, temp_campos[1], sizeof(hw->nome) - 1);
    hw->nome[sizeof(hw->nome) - 1] = '\0'; // Garante terminação nula
    
    strncpy(hw->fabricante, temp_campos[2], sizeof(hw->fabricante) - 1);
    hw->fabricante[sizeof(hw->fabricante) - 1] = '\0'; // Garante terminação nula

    hw->tipo = string_to_tipo(temp_campos[3]);
    
    if (!data_from_string(temp_campos[4], &hw->dataCompra)) {
        return false;
    }
    
    hw->valorCompra = atof(temp_campos[5]);
    hw->vidaUtilAnos = atoi(temp_campos[6]);
    
    if (!data_from_string(temp_campos[7], &hw->ultimaManutencao)) {
        return false;
    }
    
    // Compara o campo com "1" ou "1\n" para lidar com possíveis quebras de linha
    hw->obsoleto = (strcmp(temp_campos[8], "1") == 0 || strcmp(temp_campos[8], "1\n") == 0); 
    
    return true;
}

char* hardware_to_string(const Hardware* hw) {
    char* dataCompraStr = data_to_string(&hw->dataCompra);
    char* ultimaManutencaoStr = data_to_string(&hw->ultimaManutencao);
    
    // Calcula o tamanho necessário do buffer
    int size = snprintf(NULL, 0, "ID: %d | %s (%s) | Tipo: %s | Compra: %s | Última manutenção: %s | Valor: R$%.2f | Vida útil: %d anos | %s",
                        hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                        dataCompraStr, ultimaManutencaoStr, hw->valorCompra,
                        hw->vidaUtilAnos, hw->obsoleto ? "OBSOLETO" : "Ativo");
    
    char* str = malloc(size + 1);
    if (str) {
        // Formata a string de exibição
        sprintf(str, "ID: %d | %s (%s) | Tipo: %s | Compra: %s | Última manutenção: %s | Valor: R$%.2f | Vida útil: %d anos | %s",
                hw->id, hw->nome, hw->fabricante, tipo_to_string(hw->tipo),
                dataCompraStr, ultimaManutencaoStr, hw->valorCompra,
                hw->vidaUtilAnos, hw->obsoleto ? "OBSOLETO" : "Ativo");
    }
    
    free(dataCompraStr);
    free(ultimaManutencaoStr);
    return str;
}

void linkedlist_init(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void linkedlist_clear(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        // Não é mais necessário liberar nome e fabricante, pois são arrays fixos na struct
        free(current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void linkedlist_push_back(LinkedList* list, const Hardware* hw) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) return;
    
    newNode->data.id = hw->id;
    // Copia os dados da struct Hardware, incluindo nome e fabricante (agora arrays fixos)
    strncpy(newNode->data.nome, hw->nome, sizeof(newNode->data.nome) - 1);
    newNode->data.nome[sizeof(newNode->data.nome) - 1] = '\0';
    
    strncpy(newNode->data.fabricante, hw->fabricante, sizeof(newNode->data.fabricante) - 1);
    newNode->data.fabricante[sizeof(newNode->data.fabricante) - 1] = '\0';

    newNode->data.tipo = hw->tipo;
    newNode->data.dataCompra = hw->dataCompra;
    newNode->data.valorCompra = hw->valorCompra;
    newNode->data.vidaUtilAnos = hw->vidaUtilAnos;
    newNode->data.ultimaManutencao = hw->ultimaManutencao;
    newNode->data.obsoleto = hw->obsoleto;
    newNode->next = NULL;
    
    if (list->tail == NULL) {
        list->head = list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->size++;
}

Node* linkedlist_get_head(const LinkedList* list) {
    return list->head;
}

int linkedlist_get_size(const LinkedList* list) {
    return list->size;
}

// Funções de comparação para ordenação
bool compare_data_compra(const Hardware* a, const Hardware* b) {
    return data_menor_que(&a->dataCompra, &b->dataCompra);
}

bool compare_data_manutencao(const Hardware* a, const Hardware* b) {
    return data_menor_que(&a->ultimaManutencao, &b->ultimaManutencao);
}

// Funções de ordenação que agora copiam as structs Hardware de forma segura
// (pois nome e fabricante são arrays fixos, fazendo a cópia ser "profunda" para eles)
void linkedlist_bubble_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*)) {
    if (list->size < 2) return;
    
    bool swapped;
    Node *current;
    
    // Implementação de bubble sort otimizada para linked list, trocando dados
    // Esta abordagem é segura agora que Hardware não contém ponteiros para strings alocadas dinamicamente
    do {
        swapped = false;
        current = list->head;
        
        while (current != NULL && current->next != NULL) {
            if (compare(&current->next->data, &current->data)) { // Se o próximo for "menor" que o atual, troca
                Hardware temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
                swapped = true;
            }
            current = current->next;
        }
    } while (swapped);
}

void linkedlist_insertion_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*)) {
    if (list->size < 2) return;
    
    Node* sorted = NULL;
    Node* current = list->head;
    
    // Implementação de insertion sort para linked list, trocando dados
    // Esta abordagem é segura agora que Hardware não contém ponteiros para strings alocadas dinamicamente
    while (current != NULL) {
        Node* next = current->next; // Salva o próximo antes de modificar current->next
        
        if (sorted == NULL || compare(&current->data, &sorted->data)) {
            // Insere no início da lista ordenada
            current->next = sorted;
            sorted = current;
        } else {
            // Procura o local de inserção na lista ordenada
            Node* temp = sorted;
            while (temp->next != NULL && compare(&temp->next->data, &current->data)) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    
    list->head = sorted;
    // Atualiza o tail após a ordenação
    list->tail = sorted;
    while (list->tail != NULL && list->tail->next != NULL) {
        list->tail = list->tail->next;
    }
}


void sistema_init(SistemaInventario* sistema) {
    linkedlist_init(&sistema->inventario);
    sistema->proximoId = 1;
    sistema->arquivoDados = "inventario.csv"; // Nome do arquivo de dados
    sistema_carregar_dados(sistema);
    
    // Encontra o próximo ID disponível (o maior ID + 1)
    int max_id = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.id > max_id) {
            max_id = current->data.id;
        }
        current = current->next;
    }
    sistema->proximoId = max_id + 1;
}

void sistema_destroy(SistemaInventario* sistema) {
    sistema_salvar_dados(sistema); // Salva os dados antes de destruir
    linkedlist_clear(&sistema->inventario); // Libera a memória da lista
}

void sistema_carregar_dados(SistemaInventario* sistema) {
    FILE* arquivo = fopen(sistema->arquivoDados, "r");
    if (!arquivo) {
        printf("Arquivo de dados '%s' não encontrado. Criando novo inventário.\n", sistema->arquivoDados);
        return;
    }

    printf("Carregando dados do arquivo '%s'...\n", sistema->arquivoDados);
    char linha[1024]; // Buffer para ler cada linha do arquivo
    
    // Lê a linha do cabeçalho e a ignora
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        printf("Arquivo de dados vazio ou com erro.\n");
        return;
    }

    int contador = 0;
    // Loop para ler cada linha do arquivo até o fim
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        // Remove a nova linha se estiver presente
        linha[strcspn(linha, "\n")] = '\0'; 
        if (linha[0] == '\0') continue; // Ignora linhas em branco
        
        Hardware hw;
        // Tenta parsear a linha CSV para uma struct Hardware
        if (hardware_from_csv(linha, &hw)) {
            linkedlist_push_back(&sistema->inventario, &hw); // Adiciona na lista encadeada
            contador++;
        } else {
            fprintf(stderr, "Erro ao carregar linha (formato inválido): %s\n", linha);
        }
    }
    fclose(arquivo);
    printf("Dados carregados com sucesso. %d itens encontrados.\n", contador);
}

void sistema_salvar_dados(SistemaInventario* sistema) {
    FILE* arquivo = fopen(sistema->arquivoDados, "w"); // Abre o arquivo para escrita (sobrescreve se existir)
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir arquivo para salvar dados: %s\n", sistema->arquivoDados);
        return;
    }

    // Escreve o cabeçalho no arquivo CSV
    fprintf(arquivo, "ID;Nome;Fabricante;Tipo;DataCompra;Valor;VidaUtil;UltimaManutencao;Obsoleto\n");

    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        char* csv = hardware_to_csv(&current->data); // Converte a struct Hardware para string CSV
        if (csv) {
            fprintf(arquivo, "%s\n", csv); // Escreve a linha CSV no arquivo
            free(csv); // Libera a memória alocada por hardware_to_csv
        }
        current = current->next;
        contador++;
    }
    fclose(arquivo);
    printf("Dados salvos com sucesso (%d itens) no arquivo %s\n", contador, sistema->arquivoDados);
}

bool sistema_cadastrar_hardware(SistemaInventario* sistema, const char* nome, const char* fabricante, 
                               TipoHardware tipo, const Data* dataCompra, double valorCompra, 
                               int vidaUtilAnos) {
    if (nome == NULL || fabricante == NULL || strlen(nome) == 0 || strlen(fabricante) == 0) {
        fprintf(stderr, "Nome e fabricante não podem estar vazios.\n");
        return false;
    }
    if (valorCompra <= 0 || vidaUtilAnos <= 0) {
        fprintf(stderr, "Valor e vida útil devem ser positivos.\n");
        return false;
    }

    Hardware hw;
    hw.id = sistema->proximoId++; // Atribui um novo ID e incrementa
    // Copia as strings para os buffers da struct, garantindo que não excedam o tamanho
    strncpy(hw.nome, nome, sizeof(hw.nome) - 1);
    hw.nome[sizeof(hw.nome) - 1] = '\0'; // Garante terminação nula
    
    strncpy(hw.fabricante, fabricante, sizeof(hw.fabricante) - 1);
    hw.fabricante[sizeof(hw.fabricante) - 1] = '\0'; // Garante terminação nula
    
    hw.tipo = tipo;
    hw.dataCompra = *dataCompra;
    hw.valorCompra = valorCompra;
    hw.vidaUtilAnos = vidaUtilAnos;
    hw.ultimaManutencao = *dataCompra; // Inicialmente, última manutenção é a data de compra
    hw.obsoleto = false;

    linkedlist_push_back(&sistema->inventario, &hw); // Adiciona o novo hardware à lista
    printf("Hardware cadastrado com ID: %d\n", hw.id);
    return true;
}

bool sistema_registrar_manutencao(SistemaInventario* sistema, int id, const Data* dataManutencao) {
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.id == id) { // Encontra o equipamento pelo ID
            current->data.ultimaManutencao = *dataManutencao; // Atualiza a data de manutenção
            return true;
        }
        current = current->next;
    }
    return false; // Equipamento não encontrado
}

void sistema_listar_equipamentos(SistemaInventario* sistema) {
    printf("=== LISTA DE EQUIPAMENTOS (%d) ===\n", sistema->inventario.size);
    if (sistema->inventario.size == 0) {
        printf("Nenhum equipamento cadastrado.\n");
        return;
    }
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        char* str = hardware_to_string(&current->data); // Converte Hardware para string formatada
        if (str) {
            printf("%s\n", str);
            free(str); // Libera a memória alocada por hardware_to_string
        }
        current = current->next;
    }
}

void sistema_listar_por_tipo(SistemaInventario* sistema, TipoHardware tipo) {
    printf("=== EQUIPAMENTOS POR TIPO (%s) ===\n", tipo_to_string(tipo));
    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        if (current->data.tipo == tipo) { // Filtra pelo tipo de hardware
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
    LinkedList temp;
    linkedlist_init(&temp);
    
    // Copia os equipamentos para uma lista temporária para ordenação
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        linkedlist_push_back(&temp, &current->data);
        current = current->next;
    }
    
    // Ordena a lista temporária pela data de compra usando Insertion Sort
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
    
    linkedlist_clear(&temp); // Libera a memória da lista temporária
}

void sistema_listar_por_data_manutencao(SistemaInventario* sistema) {
    LinkedList temp;
    linkedlist_init(&temp);
    
    // Copia os equipamentos para uma lista temporária para ordenação
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        linkedlist_push_back(&temp, &current->data);
        current = current->next;
    }
    
    // Ordena a lista temporária pela data da última manutenção usando Bubble Sort
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
    
    linkedlist_clear(&temp); // Libera a memória da lista temporária
}

double calcular_depreciacao(const Hardware* hw, const Data* hoje) {
    // Calcula a diferença de anos entre a data de hoje e a data de compra
    int anos = hoje->ano - hw->dataCompra.ano;
    if (hoje->mes < hw->dataCompra.mes || (hoje->mes == hw->dataCompra.mes && hoje->dia < hw->dataCompra.dia)) {
        anos--; // Ajusta se o aniversário de compra ainda não ocorreu no ano atual
    }
    
    if (anos <= 0) return 0.0; // Não há depreciação se o equipamento for novo ou muito recente
    if (anos >= hw->vidaUtilAnos) return hw->valorCompra; // Depreciação total se excedeu a vida útil
    
    // Cálculo linear da depreciação
    return (hw->valorCompra / hw->vidaUtilAnos) * anos;
}

void sistema_mostrar_analise_depreciacao(SistemaInventario* sistema, const Data* hoje) {
    char* hojeStr = data_to_string(hoje);
    printf("=== ANÁLISE DE DEPRECIAÇÃO (Data base: %s) ===\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr); // Libera a string da data

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
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        // Calcula a idade do equipamento em anos
        int anos = hoje->ano - current->data.dataCompra.ano;
        if (hoje->mes < current->data.dataCompra.mes || 
            (hoje->mes == current->data.dataCompra.mes && hoje->dia < current->data.dataCompra.dia)) {
            anos--;
        }
        // Atualiza o status de obsoleto
        current->data.obsoleto = (anos >= current->data.vidaUtilAnos);
        current = current->next;
    }
}

void sistema_identificar_obsoletos(SistemaInventario* sistema, const Data* hoje) {
    sistema_atualizar_status_obsoleto(sistema, hoje); // Garante que o status de obsoleto está atualizado
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
    char* hojeStr = data_to_string(hoje);
    printf("=== EQUIPAMENTOS COM MANUTENÇÃO PENDENTE (> %d meses, Data base: %s) ===\n",
           mesesLimite, hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr);
    
    int contador = 0;
    Node* current = sistema->inventario.head;
    while (current != NULL) {
        // Calcula os meses desde a última manutenção
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

TipoHardware selecionar_tipo() {
    printf("\nSelecione o tipo de hardware:\n");
    printf("1 - Computador\n2 - Impressora\n3 - Servidor\n");
    printf("4 - Roteador\n5 - Switch\n6 - Outro\n");
    printf("Opção: ");
    
    int op;
    while (true) {
        // Verifica se a entrada é um número e está dentro do intervalo válido
        if (scanf("%d", &op) != 1 || op < 1 || op > 6) {
            limpar_buffer_entrada(); // Limpa o buffer em caso de entrada inválida
            printf("Opção inválida! Tente novamente (1-6): ");
            continue;
        }
        break; // Sai do loop se a entrada for válida
    }
    limpar_buffer_entrada(); // Limpa o restante da linha após o scanf

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
    char sep1, sep2; // Separadores para o formato da data
    while (true) {
        // Verifica o formato da data e a validade dos valores
        if (scanf("%d%c%d%c%d", &data->dia, &sep1, &data->mes, &sep2, &data->ano) != 5 || sep1 != '/' || sep2 != '/' || 
            data->dia < 1 || data->dia > 31 || data->mes < 1 || data->mes > 12 || data->ano < 1900) {
            limpar_buffer_entrada(); // Limpa o buffer em caso de entrada inválida
            printf("Data inválida! Use o formato DD/MM/AAAA: ");
            continue;
        }
        break; // Sai do loop se a data for válida
    }
    limpar_buffer_entrada(); // Limpa o restante da linha após o scanf
    return true;
}

// Função para limpar o buffer de entrada (após scanf)
void limpar_buffer_entrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void menu_principal() {
    SistemaInventario sistema;
    sistema_init(&sistema); // Inicializa o sistema de inventário
    Data hoje = obter_data_atual(); // Obtém a data atual
    char* hojeStr = data_to_string(&hoje);
    
    printf("\n=== SISTEMA DE INVENTÁRIO DE HARDWARE ===\n");
    printf("Data atual do sistema: %s\n", hojeStr ? hojeStr : "ERRO");
    if (hojeStr) free(hojeStr); // Libera a string da data

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
            // Verifica se a entrada é um número e está dentro do intervalo válido
            if (scanf("%d", &opcao) != 1 || opcao < 0 || opcao > 9) {
                limpar_buffer_entrada(); // Limpa o buffer em caso de entrada inválida
                printf("Opção inválida! Tente novamente (0-9): ");
                continue;
            }
            break; // Sai do loop se a entrada for válida
        }
        limpar_buffer_entrada(); // Limpa o restante da linha após o scanf

        switch(opcao) {
            case 1: {
                char nome[100], fabricante[100];
                printf("\n--- CADASTRO DE HARDWARE ---\n");
                printf("Nome do equipamento: ");
                fgets(nome, sizeof(nome), stdin); // Lê o nome com espaços
                nome[strcspn(nome, "\n")] = '\0'; // Remove o '\n' lido por fgets
                
                printf("Fabricante: ");
                fgets(fabricante, sizeof(fabricante), stdin); // Lê o fabricante com espaços
                fabricante[strcspn(fabricante, "\n")] = '\0'; // Remove o '\n' lido por fgets
                
                TipoHardware tipo = selecionar_tipo(); // Pede para o usuário selecionar o tipo
                Data dataCompra;
                ler_data("Data de compra", &dataCompra); // Pede a data de compra
                
                double valor;
                printf("Valor de compra (R$): ");
                while (scanf("%lf", &valor) != 1 || valor <= 0) { // Validação do valor de compra
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um valor positivo: ");
                }
                limpar_buffer_entrada();
                
                int vidaUtil;
                printf("Vida útil em anos: ");
                while (scanf("%d", &vidaUtil) != 1 || vidaUtil <= 0) { // Validação da vida útil
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                
                // Cadastra o hardware no sistema
                sistema_cadastrar_hardware(&sistema, nome, fabricante, tipo, &dataCompra, valor, vidaUtil);
                break;
            }
            case 2: {
                printf("\n--- REGISTRAR MANUTENÇÃO ---\n");
                int id;
                printf("ID do equipamento: ");
                while (scanf("%d", &id) != 1 || id <= 0) { // Validação do ID
                    limpar_buffer_entrada();
                    printf("ID inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                Data dataManutencao;
                ler_data("Data da manutenção", &dataManutencao); // Pede a data da manutenção
                if (sistema_registrar_manutencao(&sistema, id, &dataManutencao)) {
                    printf("Manutenção registrada com sucesso!\n");
                } else {
                    printf("Equipamento não encontrado!\n");
                }
                break;
            }
            case 3:
                sistema_listar_equipamentos(&sistema); // Lista todos os equipamentos
                break;
            case 4: {
                TipoHardware tipo = selecionar_tipo(); // Pede o tipo para filtrar
                sistema_listar_por_tipo(&sistema, tipo); // Lista por tipo
                break;
            }
            case 5:
                sistema_listar_por_data_compra(&sistema); // Lista ordenado por data de compra
                break;
            case 6:
                sistema_listar_por_data_manutencao(&sistema); // Lista ordenado por data de manutenção
                break;
            case 7:
                sistema_mostrar_analise_depreciacao(&sistema, &hoje); // Mostra análise de depreciação
                break;
            case 8:
                sistema_identificar_obsoletos(&sistema, &hoje); // Identifica equipamentos obsoletos
                break;
            case 9: {
                printf("\n--- RELATÓRIO DE MANUTENÇÃO PENDENTE ---\n");
                int meses;
                printf("Informe o limite de meses sem manutenção: ");
                while (scanf("%d", &meses) != 1 || meses <= 0) { // Validação do limite de meses
                    limpar_buffer_entrada();
                    printf("Valor inválido! Digite um número positivo: ");
                }
                limpar_buffer_entrada();
                sistema_relatorio_manutencao_pendente(&sistema, &hoje, meses); // Gera relatório de manutenção pendente
                break;
            }
            case 0:
                printf("\nSalvando dados e saindo...\n");
                break;
        }
    } while (opcao != 0);
    
    sistema_destroy(&sistema); // Libera os recursos do sistema
}

int main() {
    // A função menu_principal() contém o loop principal do programa e a lógica
    menu_principal(); 
    return 0; // Retorna 0 para indicar execução bem-sucedida
}
