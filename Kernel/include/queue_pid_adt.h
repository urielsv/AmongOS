#ifndef queue_pid_h
#define queue_pid_h

#include <stdint.h>
#include <stdbool.h>

typedef struct queue_pid_cdt* queue_pid_adt;

// creates a new empty queue
queue_pid_adt create_queue(void);

// adds a pid to the rear of the queue
void enqueue(queue_pid_adt queue, int64_t pid);

// removes and returns the pid at the front of the queue
// returns -1 if queue is empty
int64_t dequeue(queue_pid_adt queue);

// returns the pid at the front without removing it
// returns -1 if queue is empty
int64_t peek(queue_pid_adt queue);

// returns true if the queue is empty, false otherwise
bool is_empty(queue_pid_adt queue);

// returns the current number of elements in the queue
uint64_t get_queue_size(queue_pid_adt queue);

// removes all elements from the queue
void clear_queue(queue_pid_adt queue);

// checks if a specific pid exists in the queue
bool contains_pid(queue_pid_adt queue, int64_t pid);

// frees all memory associated with the queue
void destroy_queue(queue_pid_adt queue);

void dequeue_pid(queue_pid_adt queue, int64_t pid);

#endif // queue_pid_h