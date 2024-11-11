#include <queuePidADT.h>
#include <memman.h>
#include <stddef.h>
#include <stdint.h>
#include <buddy_memman.h>

typedef struct node_pid {
    int64_t pid;
    struct node_pid* next;
} node_pid_t;

typedef struct queuePIDCDT {
    node_pid_t* front;    
    node_pid_t* rear;     
    uint64_t size;
} queuePIDCDT_t;

queuePIDADT createQueue() {
    queuePIDADT queue = b_alloc(sizeof(queuePIDCDT_t));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(queuePIDADT queue, int64_t pid) {
    node_pid_t* newNode = b_alloc(sizeof(node_pid_t));
    newNode->pid = pid;
    newNode->next = NULL;
    
    if (queue->rear == NULL) {  
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
    queue->size++;
}

int64_t dequeue(queuePIDADT queue) {
    if (queue->front == NULL) {
        return -1;  
    }
    
    node_pid_t* temp = queue->front;
    int64_t pid = temp->pid;
    
    queue->front = queue->front->next;
    if (queue->front == NULL) {  
        queue->rear = NULL;
    }
    
    b_free(temp);
    queue->size--;
    return pid;
}

int64_t peek(queuePIDADT queue) {
    if (queue->front == NULL) {
        return -1;
    }
    return queue->front->pid;
}

bool isEmpty(queuePIDADT queue) {
    return (queue->size == 0);
}

uint64_t getQueueSize(queuePIDADT queue) {
    return queue->size;
}

void clearQueue(queuePIDADT queue) {
    while (queue->front != NULL) {
        node_pid_t* temp = queue->front;
        queue->front = queue->front->next;
        b_free(temp);
    }
    queue->rear = NULL;
    queue->size = 0;
}

bool containsPID(queuePIDADT queue, int64_t pid) {
    node_pid_t* current = queue->front;
    while (current != NULL) {
        if (current->pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void destroyQueue(queuePIDADT queue) {
    clearQueue(queue);
    mem_free(queue);
}

void dequeue_pid(queuePIDADT queue, int64_t pid) {
    if (queue->front == NULL) {
        return;
    }
    
    node_pid_t* current = queue->front;
    node_pid_t* prev = NULL;
    
    while (current != NULL) {
        if (current->pid == pid) {
            if (prev == NULL) {
                queue->front = current->next;
            } else {
                prev->next = current->next;
            }
            if (queue->rear == current) {
                queue->rear = prev;
            }
            mem_free(current);
            queue->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}