#include <buddy_memman.h>
#include <stddef.h>

#define min 5
#define levels 19

#define free 0
#define taken 1

#define block_size(l) (1<<(l))
#define buddy_of(b)  ((header_t *)((uint64_t) base +  (((uint64_t)((void *)(b) - base)) ^ (1 << (b->level)))))
#define hide_header_t(b) ((void  *)(b) + sizeof(header_t))
#define show_header_t(m) ((header_t *) ((m) - sizeof(header_t)))

typedef struct header_t {
	uint8_t level;
	uint8_t status;
	struct header_t * prev;
	struct header_t * next;
} header_t;

void * base;
uint8_t max;
header_t * free_lists[levels];
uint64_t total_mem;
uint64_t used_mem;

uint8_t get_level(uint64_t size);
header_t *new_header_t(void * address, uint8_t level, header_t * next);
void free_lists_remove(header_t * header);
void split(uint8_t idx);
header_t *merge(header_t * block, header_t * buddy);


void b_init(void *initial_address, uint64_t size){
	base = initial_address;
	max = get_level(size);
    total_mem = size;
	free_lists[max - 1] = new_header_t(base, max, NULL);
}

void * b_alloc(uint64_t size) {
    uint8_t level = get_level(size+sizeof(header_t));
    if (level < min-1){
        level = min-1;
    }
    if (level >= max){
        return NULL;
    }
    uint8_t to_split = level;
    while(1){
        if (to_split>=max){
            return NULL;
        }
        if (free_lists[to_split] != NULL){
            break;
        }
        to_split++;
    }
    while(to_split>level){
        split(to_split);
        to_split--;
    }
    header_t *block = free_lists[level];
	free_lists_remove(block);
	block->status = taken;
    used_mem += block_size(block->level);
    return hide_header_t(block);
}

void b_free(void * ptr) {
    if (ptr==NULL){
        return;
    }
	header_t * block = show_header_t(ptr);
    block->status = free;
    used_mem -= block_size(block->level);
    for (header_t * buddy = buddy_of(block); buddy->status==free && buddy->level==block->level && block->level < max; buddy = buddy_of(block)){
        block = merge(block, buddy);
    }
	free_lists[block->level - 1] = new_header_t(block, block->level, free_lists[block->level - 1]);
}

 void memory_info(){
    // print_string("buddy memory manager\n");
    // print_string("total: ");
    // print_int(total_mem);
    // print_string("\n_disponible: ");
    // print_int(total_mem - used_mem);
    // print_string("\n_en uso: ");
    // print_int(used_mem);
    // print_string("\n");
 }



uint8_t get_level(uint64_t size){
    uint8_t i = 0;
    while (size >>= 1){
        i++;
    }
    return i;
}

header_t * new_header_t(void * address, uint8_t level, header_t *next) {
	header_t * header = (header_t *) address;
	header->level = level;
	header->status = free;
	header->prev = NULL;
	header->next = next;
	if (next != NULL) {
		next->prev = header;
	}
	return header;
}

void free_lists_remove(header_t * header) {
	if (header->prev == NULL){
        free_lists[header->level - 1] = header->next;
    } else{
		header->prev->next = header->next;
    }
	if (header->next != NULL){
		header->next->prev = header->prev;
    }
}

void split(uint8_t idx) {
	header_t *block = free_lists[idx];
	free_lists_remove(block);
	header_t *buddy = (header_t *) ((void*)block + (1 << idx));
	new_header_t(buddy, idx, free_lists[idx - 1]);
	free_lists[idx - 1] = new_header_t(block, idx, buddy);
}

header_t * merge(header_t *block, header_t *buddy) {
	free_lists_remove(buddy);
	header_t *block_start = block < buddy ? block : buddy;
	block_start->level++;
	return block_start;
}
