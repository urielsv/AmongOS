#ifndef linkedlistadt_h
#define linkedlistadt_h

#include <stdint.h>


typedef struct linked_list_cdt_t * linked_list_adt;
typedef struct node_t node_t; 

typedef struct node_t {
    void * process;
    node_t * next;
    node_t * prev;
} node_t;

#include <stdint.h>

linked_list_adt create_linked_list();
void add_node(linked_list_adt list, void * process);
void remove_node(linked_list_adt list, void * process);
void * get_node(linked_list_adt list, void * process);
node_t * get_first_node(linked_list_adt list);
void destroy_linked_list(linked_list_adt list);
void * get_next_node(linked_list_adt list);
void * swap_to_last(linked_list_adt list, void * process);
void remove_all_nodes(linked_list_adt list, void * process);
uint64_t get_size(linked_list_adt list);
void remove_first_node(linked_list_adt list);
int8_t is_empty_list(linked_list_adt list);
void start_iterator(linked_list_adt list);
uint8_t has_next(linked_list_adt list);
void * get_next(linked_list_adt list);

#endif // linkedlistadt_h
