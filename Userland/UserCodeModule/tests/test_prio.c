// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <tests.h>
#include <syscalls.h>
#include <definitions.h>
#define minor_wait 10 
#define wait 2000000     
#define total_processes 5

static void endless_loop_print_wrap() {
  endless_loop_print(minor_wait);
  
}


int64_t prio[total_processes] = {low, low_medium, medium, high_medium, high};

void test_prio() {
  int64_t pids[total_processes];
  
  uint64_t i;

  for (i = 0; i < total_processes; i++)
    pids[i] = exec(( void*) &endless_loop_print_wrap, NULL, 0, "test", prio[i]);

  bussy_wait(wait);
  printf("\n_changing priorities...\n");

  for (i = 0; i < total_processes; i++)
    set_priority(pids[i], prio[i]);

  bussy_wait(wait);
  printf("\n_blocking...\n");

  for (i = 0; i < total_processes; i++)
    block(pids[i]);

  printf("\n_changing priorities while blocked...\n");

  for (i = 0; i < total_processes; i++)
    set_priority(pids[i], medium);


  bussy_wait(wait/3);
  printf("\n_unblocking...\n");

  for (i = 0; i < total_processes; i++)
    unblock(pids[i]);

  bussy_wait(wait);
  printf("\n_killing...\n");

  for (i = 0; i < total_processes; i++)
    kill(pids[i]);
}
