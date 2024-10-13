#ifndef LINKEDLISTADT_H
#define LINKEDLISTADT_H

typedef linkedListCDT * linkedListADT;
typedef struct {
    void * process;
    node_t * next;
    note_t * prev;
} node_t;


linkedListADT createLinkedList();
void addNode(linkedListADT list, void * process);
void removeNode(linkedListADT list, void * process);
void * getNode(linkedListADT list, void * process);
void destroyLinkedList(linkedListADT list);

#endif // LINKEDLISTADT_H