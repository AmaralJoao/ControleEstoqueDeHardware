#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "hardware.h"
#include "linkedList.h"
#include <stdbool.h>

// Interface do repositório
typedef struct RepositoryInterface {
    bool (*carregar)(void* self, LinkedList* list);
    bool (*salvar)(void* self, const LinkedList* list);
    bool (*adicionar)(void* self, const Hardware* hw);
    bool (*atualizar)(void* self, const Hardware* hw);
    bool (*remover)(void* self, int id);
    Hardware* (*buscar_por_id)(void* self, int id);
    void (*destruir)(void* self);
} RepositoryInterface;

// Estrutura do repositório
typedef struct {
    void* implementacao;
    const RepositoryInterface* interface;
} Repository;

Repository* criar_repositorio_csv(const char* filename);
void destruir_repositorio(Repository* repo);

#endif 