// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <tests.h>
#include <syscalls.h>
#include <string.h>

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

  if ((max_memory = satoi(argv[0])) <= 0) {
    return -1;
  }
   
  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_PROCESSES && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].size = 1000;
      mm_rqs[rq].address = mem_alloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address) {
        printf("a!    ");
        total += mm_rqs[rq].size;
        rq++;
      }
      // Agregamos este set ya que intentamos pedir memoria y no nos la dieron, entonces seteamos el size en 0 para que no hayan problemas con el memcheck
      else {
        mm_rqs[rq].size = 0;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
        printf("s!    ");
      }  

    }

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          return -1;
        } 
        printf("c!    ");
      }

    // mem_free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address){
        mem_free(mm_rqs[i].address);
        printf("f!    ");
      }
  }
}