#include <stdint.h>
#include <stdio.h>
#include <tests.h>
#include <syscalls.h>
#include <definitions.h>
#define MINOR_WAIT 5 // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 50      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice


#define TOTAL_PROCESSES 5
#define LOWEST 0  
#define LOW_MEDIUM 1  
#define MEDIUM 2 
#define MEDIUM_HIGH 3  
#define HIGHEST 4 

static void endless_loop_print_wrap() {
  endless_loop_print(WAIT);
  //endless_loop();
}


int64_t prio[TOTAL_PROCESSES] = {LOWEST, LOW_MEDIUM, MEDIUM, MEDIUM_HIGH, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = exec(( void*) &endless_loop_print_wrap, NULL, 0, "test", prio[i]);

  bussy_wait(WAIT);
  printf("\nCHANGING PRIORITIES...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  printf("\nBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    block(pids[i]);

  printf("CHANGING PRIORITIES WHILE BLOCKED...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    my_nice(pids[i], MEDIUM);

  printf("UNBLOCKING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    unblock(pids[i]);

  bussy_wait(WAIT);
  printf("\nKILLING...\n");

  for (i = 0; i < TOTAL_PROCESSES; i++)
    kill(pids[i]);
}
