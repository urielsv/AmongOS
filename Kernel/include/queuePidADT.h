#ifndef QUEUE_PID_H
#define QUEUE_PID_H

#include <stdint.h>
#include <stdbool.h>

typedef struct queuePIDCDT* queuePIDADT;

// Creates a new empty queue
queuePIDADT createQueue(void);

// Adds a PID to the rear of the queue
void enqueue(queuePIDADT queue, int64_t pid);

// Removes and returns the PID at the front of the queue
// Returns -1 if queue is empty
int64_t dequeue(queuePIDADT queue);

// Returns the PID at the front without removing it
// Returns -1 if queue is empty
int64_t peek(queuePIDADT queue);

// Returns true if the queue is empty, false otherwise
bool isEmpty(queuePIDADT queue);

// Returns the current number of elements in the queue
uint64_t getQueueSize(queuePIDADT queue);

// Removes all elements from the queue
void clearQueue(queuePIDADT queue);

// Checks if a specific PID exists in the queue
bool containsPID(queuePIDADT queue, int64_t pid);

// Frees all memory associated with the queue
void destroyQueue(queuePIDADT queue);

#endif // QUEUE_PID_H