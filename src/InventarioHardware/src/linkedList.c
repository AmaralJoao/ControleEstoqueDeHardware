#include "linkedList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void linkedlist_init(LinkedList* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void linkedlist_clear(LinkedList* list) {
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
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

void linkedlist_bubble_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*)) {
    if (list->size < 2) return;
    
    bool swapped;
    Node *current;
    
    do {
        swapped = false;
        current = list->head;
        
        while (current != NULL && current->next != NULL) {
            if (compare(&current->next->data, &current->data)) {
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
    
    while (current != NULL) {
        Node* next = current->next;
        
        if (sorted == NULL || compare(&current->data, &sorted->data)) {
            current->next = sorted;
            sorted = current;
        } else {
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
    list->tail = sorted;
    while (list->tail != NULL && list->tail->next != NULL) {
        list->tail = list->tail->next;
    }
}