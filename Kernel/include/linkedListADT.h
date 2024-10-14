#ifndef LINKEDLISTADT_H
#define LINKEDLISTADT_H

typedef struct linkedListCDT_t * linkedListADT;
typedef struct node_t node_t; 

typedef struct node_t {
    void * process;
    struct node_t * next;
    struct node_t * prev;
} node_t;

#include <stdint.h>

linkedListADT createLinkedList();
void addNode(linkedListADT list, void * process);
void removeNode(linkedListADT list, void * process);
void * getNode(linkedListADT list, void * process);
void destroyLinkedList(linkedListADT list);

#endif // LINKEDLISTADT_H
