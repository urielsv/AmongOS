#include "../include/linkedListADT.h"
#include "../include/memman.h"

typedef struct linkedListCDT_t {
    node_t * head;
    node_t * tail;
    node_t * current;
    int size;
} linkedListCDT_t;


linkedListADT createLinkedList() {
    linkedListADT list = mem_alloc(sizeof(linkedListCDT_t));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->size = 0;
    return list;
}

void addNode(linkedListADT list, void * process) {
    node_t * node = mem_alloc(sizeof(node_t));
    node->process = process;
    node->next = NULL;
    if (list->head == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void removeNode(linkedListADT list, void * process) {
    node_t * prev = NULL;
    node_t * current = list->head;
    while (current != NULL) {
        if (current->process == process) {
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            if (current == list->tail) {
                list->tail = prev;
            }
            list->size--;
            mem_free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

void * getNode(linkedListADT list, void * process) {
    node_t * current = list->head;
    while (current != NULL) {
        if (current->process == process) {
            return current->process;
        }
        current = current->next;
    }
    return NULL;
}

void destroyLinkedList(linkedListADT list) {
    node_t * current = list->head;
    node_t * next;
    while (current != NULL) {
        next = current->next;
        mem_free(current);
        current = next;
    }
    mem_free(list);
}

