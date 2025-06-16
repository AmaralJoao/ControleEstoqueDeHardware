#include "menu.h"
#include "repository.h"
#include <stdio.h> 
#include <windows.h>


// gcc src/*.c -o inventario -I include
// ./inventario
int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Cria o repositório CSV
    Repository* repo = criar_repositorio_csv("output/inventario.csv");
    if (!repo) {
        fprintf(stderr, "Falha ao criar repositório\n");
        return 1;
    }

    // Inicializa e executa o menu
    menu_principal(repo);

    // Limpeza
    destruir_repositorio(repo);
    return 0;
}