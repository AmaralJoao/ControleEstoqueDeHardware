#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "hardware.h"

typedef struct Node {
    Hardware data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} LinkedList;

void linkedlist_init(LinkedList* list);
void linkedlist_clear(LinkedList* list);
void linkedlist_push_back(LinkedList* list, const Hardware* hw);
Node* linkedlist_get_head(const LinkedList* list);
int linkedlist_get_size(const LinkedList* list);
void linkedlist_bubble_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*));
void linkedlist_insertion_sort(LinkedList* list, bool (*compare)(const Hardware*, const Hardware*));

#endif