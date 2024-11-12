#include <queue_pid_adt.h>
#include <stddef.h>
#include <stdint.h>
#include <buddy_memman.h>

typedef struct node_pid {
    int64_t pid;
    struct node_pid* next;
} node_pid_t;

typedef struct queue_pid_cdt {
    node_pid_t* front;    
    node_pid_t* rear;     
    uint64_t size;
} queue_pid_cdt_t;

queue_pid_adt create_queue() {
    queue_pid_adt queue = b_alloc(sizeof(queue_pid_cdt_t));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(queue_pid_adt queue, int64_t pid) {
    node_pid_t* new_node = b_alloc(sizeof(node_pid_t));
    new_node->pid = pid;
    new_node->next = NULL;
    
    if (queue->rear == NULL) {  
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    queue->size++;
}

int64_t dequeue(queue_pid_adt queue) {
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

int64_t peek(queue_pid_adt queue) {
    if (queue->front == NULL) {
        return -1;
    }
    return queue->front->pid;
}

bool is_empty(queue_pid_adt queue) {
    return (queue->size == 0);
}

uint64_t get_queue_size(queue_pid_adt queue) {
    return queue->size;
}

void clear_queue(queue_pid_adt queue) {
    while (queue->front != NULL) {
        node_pid_t* temp = queue->front;
        queue->front = queue->front->next;
        b_free(temp);
    }
    queue->rear = NULL;
    queue->size = 0;
}

bool contains_pid(queue_pid_adt queue, int64_t pid) {
    node_pid_t* current = queue->front;
    while (current != NULL) {
        if (current->pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void destroy_queue(queue_pid_adt queue) {
    clear_queue(queue);
    b_free(queue);
}

void dequeue_pid(queue_pid_adt queue, int64_t pid) {
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
            b_free(current);
            queue->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}