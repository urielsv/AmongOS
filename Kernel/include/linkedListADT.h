#ifndef LINKEDLISTADT_H
#define LINKEDLISTADT_H

#include <stdint.h>

typedef struct linkedListCDT_t * linkedListADT;
typedef struct node_t node_t; 

typedef struct node_t {
    void * process;
    node_t * next;
    node_t * prev;
} node_t;

#include <stdint.h>

linkedListADT createLinkedList();
void addNode(linkedListADT list, void * process);
void removeNode(linkedListADT list, void * process);
void * getNode(linkedListADT list, void * process);
node_t * getFirstNode(linkedListADT list);
void destroyLinkedList(linkedListADT list);
void * getNextNode(linkedListADT list);
void * swapToLast(linkedListADT list, void * process);
void removeAllNodes(linkedListADT list, void * process);
uint64_t getSize(linkedListADT list);
void removeFirstNode(linkedListADT list);

#endif // LINKEDLISTADT_H
