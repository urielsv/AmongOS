#include "../include/linked_list_adt.h"
#include <stddef.h>
#include <io.h>
#include <buddy_memman.h>

    typedef struct linked_list_cdt_t {
        node_t * head;
        node_t * tail;
        node_t * current;
        node_t * iterator;
        int size;
    } linked_list_cdt_t;


linked_list_adt create_linked_list() {
    linked_list_adt list = b_alloc(sizeof(linked_list_cdt_t));
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
    list->iterator = NULL;
    list->size = 0;
    return list;
}

void add_node(linked_list_adt list, void * process) {
    node_t * node = b_alloc(sizeof(node_t));
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

void remove_all_nodes(linked_list_adt list, void * process) {
    node_t * prev = NULL;
    node_t * current = list->head;
    
    while (current != NULL) {
        if (current->process == process) {
            node_t * next = current->next;
            
            if (prev == NULL) {
                // deleting from head
                list->head = next;
            } else {
                prev->next = next;
            }
            
            if (current == list->tail) {
                list->tail = prev;
            }
            
            b_free(current);
            list->size--;
            
            current = next;  
        } else {
            prev = current;
            current = current->next;
        }
    }
    
    if (list->size == 0) {
        list->head = NULL;
        list->tail = NULL;
    }
}
void remove_node(linked_list_adt list, void * process) {
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
            b_free(current);

            return;
        }
        prev = current;
        current = current->next;
    }

}
node_t * get_first_node(linked_list_adt list) {
    return list->head;
}

void * swap_to_last(linked_list_adt list, void * process) {
    node_t * prev = NULL;
    node_t * current = list->head;
    while (current != NULL) {
        if (current->process == process) {
            if (current == list->tail) {
                return process;
            }
            if (prev == NULL) {
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            list->tail->next = current;
            list->tail = current;
            current->next = NULL;
            return process;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

void * get_node(linked_list_adt list, void * process) {
    node_t * current = list->head;
    while (current != NULL) {
        if (current->process == process) {
            return current->process;
        }
        current = current->next;
    }
    return NULL;
}

void * get_next_node(linked_list_adt list) {
    if (list->current == NULL) {
        list->current = list->head;
    } else {
        list->current = list->current->next;
    }
    return list->current;
}

void destroy_linked_list(linked_list_adt list) {
    node_t * current = list->head;
    node_t * next;
    while (current != NULL) {
        next = current->next;
        b_free(current);
        current = next;
    }
    b_free(list);
}

void remove_first_node(linked_list_adt list) {
    if (list->head == NULL) {
        return;
    }
    node_t * next = list->head->next;
    b_free(list->head);
    list->head = next;
    list->size--;
}

uint64_t get_size(linked_list_adt list) {
    return list->size;
}

int8_t is_empty_list(linked_list_adt list) {
    return list->size == 0;
}

void start_iterator(linked_list_adt list) {
    list->iterator = list->head;
}

uint8_t has_next(linked_list_adt list) {
    return list->iterator != NULL;
}

void * get_next(linked_list_adt list) {
    if (list->iterator == NULL) {
        return NULL;
    }
    void * process = list->iterator->process;
    list->iterator = list->iterator->next;
    return process;
}