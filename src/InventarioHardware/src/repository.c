#include "repository.h"
#include "hardware.h"
#include "linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* filename;
} CsvRepository;

static bool csv_carregar(void* self, LinkedList* list) {
    CsvRepository* repo = (CsvRepository*)self;
    FILE* arquivo = fopen(repo->filename, "r");
    if (!arquivo) return false;

    char linha[1024];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return false;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        linha[strcspn(linha, "\n")] = '\0';
        if (linha[0] == '\0') continue;
        
        Hardware hw;
        if (hardware_from_csv(linha, &hw)) {
            linkedlist_push_back(list, &hw);
        }
    }
    fclose(arquivo);
    return true;
}

static bool csv_salvar(void* self, const LinkedList* list) {
    CsvRepository* repo = (CsvRepository*)self;
    FILE* arquivo = fopen(repo->filename, "w");
    if (!arquivo) return false;

    fprintf(arquivo, "ID;Nome;Fabricante;Tipo;DataCompra;Valor;VidaUtil;UltimaManutencao;Obsoleto\n");

    Node* current = list->head;
    while (current != NULL) {
        char* csv = hardware_to_csv(&current->data);
        if (csv) {
            fprintf(arquivo, "%s\n", csv);
            free(csv);
        }
        current = current->next;
    }
    fclose(arquivo);
    return true;
}

static bool csv_adicionar(void* self, const Hardware* hw) {
    // Implementação simplificada para exemplo
    CsvRepository* repo = (CsvRepository*)self;
    LinkedList temp;
    linkedlist_init(&temp);
    csv_carregar(repo, &temp);
    linkedlist_push_back(&temp, hw);
    bool resultado = csv_salvar(repo, &temp);
    linkedlist_clear(&temp);
    return resultado;
}

static void csv_destruir(void* self) {
    free(self);
}

static const RepositoryInterface csv_interface = {
    .carregar = csv_carregar,
    .salvar = csv_salvar,
    .adicionar = csv_adicionar,
    .atualizar = NULL,
    .remover = NULL,
    .buscar_por_id = NULL,
    .destruir = csv_destruir
};

Repository* criar_repositorio_csv(const char* filename) {
    CsvRepository* impl = malloc(sizeof(CsvRepository));
    if (!impl) return NULL;
    
    impl->filename = filename;
    
    Repository* repo = malloc(sizeof(Repository));
    if (!repo) {
        free(impl);
        return NULL;
    }
    
    repo->implementacao = impl;
    repo->interface = &csv_interface;
    
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