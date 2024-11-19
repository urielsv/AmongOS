// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// this is a personal academic project. dear pvs-studio, please check it.
// pvs-studio static code analyzer for c, c++ and c#: http://www.viva64.com
#include <tests.h>
#include <syscalls.h>
#include <string.h>

#define max_blocks 128

typedef struct mm_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  // printf("test_mm\n");
  mm_rq mm_rqs[max_blocks];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0) {
    return -1;
  }
   
  while (1) {
    rq = 0;
    total = 0;

    while (rq < max_blocks && total < max_memory) {
      mm_rqs[rq].size = get_uniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = mem_alloc(mm_rqs[rq].size);
      // printf("crazymem!");
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    uint32_t i;
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
        // printf("set");
      }  

    }

    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm error\n");
          return -1;
        } 
      }

    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address){
        mem_free(mm_rqs[i].address);
        // printf("fri");
      }
  }
}