// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/test_mm.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  printf("test_mm\n");

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;
printf("llega despues del argc\n");

  // if ((max_memory = satoi(argv[0])) <= 0) {
  //   printf("muere en satoio\n");

  //   return -1;
  // }
  max_memory = 10000000;
  while (1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = mem_alloc(mm_rqs[rq].size);
      printf("reservando memoria\n");

      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }
    printf("termine de reservar memoria\n");
    printf("number of rqs: %d\n",rq);
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
        printf("seteando memoria\n");
      }          
          

    }

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        } else {
          printf("test_mm: Malloc check OK\n");
        }
      }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        mem_free(mm_rqs[i].address);
  }
}