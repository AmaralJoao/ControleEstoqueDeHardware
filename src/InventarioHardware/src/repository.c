#include "repository.h"
#include "hardware.h"
#include "linkedList.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* filename;
} CsvRepository;

static bool csv_carregar(void* self, LinkedList* list) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    FILE* arquivo = fopen(repo->filename, "r");
    if (!arquivo) {
        cronometro_imprimir("CSV - Carregar dados (falha)", cronometro_parar(&crono));
        return false;
    }

    char linha[1024];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        cronometro_imprimir("CSV - Carregar dados (vazio)", cronometro_parar(&crono));
        return false;
    }

    int contador = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        if (linha[0] == '\0') continue;
        
        Hardware hw;
        if (hardware_from_csv(linha, &hw)) {
            linkedlist_push_back(list, &hw);
            contador++;
        }
    }
    fclose(arquivo);
    
    double tempo = cronometro_parar(&crono);
    printf("[CSV] Carregados %d itens - ", contador);
    cronometro_imprimir("Carregar dados", tempo);
    return true;
}

static bool csv_salvar(void* self, const LinkedList* list) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    FILE* arquivo = fopen(repo->filename, "w");
    if (!arquivo) {
        cronometro_imprimir("CSV - Salvar dados (falha)", cronometro_parar(&crono));
        return false;
    }

    fprintf(arquivo, "ID;Nome;Fabricante;Tipo;DataCompra;Valor;VidaUtil;UltimaManutencao;Obsoleto\n");

    int contador = 0;
    Node* current = list->head;
    while (current != NULL) {
        char* csv = hardware_to_csv(&current->data);
        if (csv) {
            fprintf(arquivo, "%s\n", csv);
            free(csv);
            contador++;
        }
        current = current->next;
    }
    fclose(arquivo);
    
    double tempo = cronometro_parar(&crono);
    printf("[CSV] Salvos %d itens - ", contador);
    cronometro_imprimir("Salvar dados", tempo);
    return true;
}

static bool csv_adicionar(void* self, const Hardware* hw) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    LinkedList temp;
    linkedlist_init(&temp);
    
    if (!csv_carregar(repo, &temp)) {
        linkedlist_clear(&temp);
        cronometro_imprimir("CSV - Adicionar (falha ao carregar)", cronometro_parar(&crono));
        return false;
    }
    
    linkedlist_push_back(&temp, hw);
    bool resultado = csv_salvar(repo, &temp);
    linkedlist_clear(&temp);
    
    double tempo = cronometro_parar(&crono);
    cronometro_imprimir("Adicionar hardware", tempo);
    return resultado;
}

static bool csv_atualizar(void* self, const Hardware* hw) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    LinkedList temp;
    linkedlist_init(&temp);
    
    if (!csv_carregar(repo, &temp)) {
        linkedlist_clear(&temp);
        cronometro_imprimir("CSV - Atualizar (falha ao carregar)", cronometro_parar(&crono));
        return false;
    }
    
    Node* current = temp.head;
    while (current != NULL) {
        if (current->data.id == hw->id) {
            current->data = *hw;
            bool resultado = csv_salvar(repo, &temp);
            linkedlist_clear(&temp);
            
            double tempo = cronometro_parar(&crono);
            cronometro_imprimir("Atualizar hardware", tempo);
            return resultado;
        }
        current = current->next;
    }
    
    linkedlist_clear(&temp);
    cronometro_imprimir("CSV - Atualizar (não encontrado)", cronometro_parar(&crono));
    return false;
}

static bool csv_remover(void* self, int id) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    LinkedList temp;
    linkedlist_init(&temp);
    
    if (!csv_carregar(repo, &temp)) {
        linkedlist_clear(&temp);
        cronometro_imprimir("CSV - Remover (falha ao carregar)", cronometro_parar(&crono));
        return false;
    }
    
    Node* prev = NULL;
    Node* current = temp.head;
    while (current != NULL) {
        if (current->data.id == id) {
            if (prev == NULL) {
                temp.head = current->next;
            } else {
                prev->next = current->next;
            }
            if (current == temp.tail) {
                temp.tail = prev;
            }
            
            bool resultado = csv_salvar(repo, &temp);
            free(current);
            linkedlist_clear(&temp);
            
            double tempo = cronometro_parar(&crono);
            cronometro_imprimir("Remover hardware", tempo);
            return resultado;
        }
        prev = current;
        current = current->next;
    }
    
    linkedlist_clear(&temp);
    cronometro_imprimir("CSV - Remover (não encontrado)", cronometro_parar(&crono));
    return false;
}

static Hardware* csv_buscar_por_id(void* self, int id) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* repo = (CsvRepository*)self;
    LinkedList temp;
    linkedlist_init(&temp);
    
    if (!csv_carregar(repo, &temp)) {
        linkedlist_clear(&temp);
        cronometro_imprimir("CSV - Buscar (falha ao carregar)", cronometro_parar(&crono));
        return NULL;
    }
    
    Node* current = temp.head;
    while (current != NULL) {
        if (current->data.id == id) {
            Hardware* copia = malloc(sizeof(Hardware));
            if (copia) {
                *copia = current->data;
            }
            linkedlist_clear(&temp);
            
            double tempo = cronometro_parar(&crono);
            cronometro_imprimir("Buscar hardware", tempo);
            return copia;
        }
        current = current->next;
    }
    
    linkedlist_clear(&temp);
    cronometro_imprimir("CSV - Buscar (não encontrado)", cronometro_parar(&crono));
    return NULL;
}

static void csv_destruir(void* self) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    free(self);
    
    cronometro_imprimir("Destruir repositório", cronometro_parar(&crono));
}

static const RepositoryInterface csv_interface = {
    .carregar = csv_carregar,
    .salvar = csv_salvar,
    .adicionar = csv_adicionar,
    .atualizar = csv_atualizar,
    .remover = csv_remover,
    .buscar_por_id = csv_buscar_por_id,
    .destruir = csv_destruir
};

Repository* criar_repositorio_csv(const char* filename) {
    Cronometro crono;
    cronometro_iniciar(&crono);
    
    CsvRepository* impl = malloc(sizeof(CsvRepository));
    if (!impl) {
        cronometro_imprimir("Criar repositório (falha alocação)", cronometro_parar(&crono));
        return NULL;
    }
    
    impl->filename = filename;
    
    Repository* repo = malloc(sizeof(Repository));
    if (!repo) {
        free(impl);
        cronometro_imprimir("Criar repositório (falha alocação)", cronometro_parar(&crono));
        return NULL;
    }
    
    repo->implementacao = impl;
    repo->interface = &csv_interface;
    
    cronometro_imprimir("Criar repositório", cronometro_parar(&crono));
    return repo;
}

void destruir_repositorio(Repository* repo) {
    if (repo) {
        if (repo->interface && repo->interface->destruir) {
            repo->interface->destruir(repo->implementacao);
        }
        free(repo);
    }
}