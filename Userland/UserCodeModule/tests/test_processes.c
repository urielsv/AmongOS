#include <stdio.h>
#include <tests.h>
#include <stdint.h>
#include <syscalls.h>
#include <definitions.h>

typedef struct P_rq {
  int32_t pid;
  state_t state;
} p_rq;


static void endless_loop_wrap() {
  endless_loop();
}

int64_t test_processes(uint64_t argc, char *argv[]) {
  printf("a\n");
  
  uint8_t rq;
  uint8_t alive = 0;
   uint8_t action;
  uint64_t maxes;
  char *argvAux[] = {0};

  if (argc != 1)
    return -1;

  if ((maxes = satoi(argv[0])) <= 0)
    return -1;

  p_rq p_rqs[maxes];


  while (1) {
    

     // Create maxes processes
    for (rq = 0; rq < maxes; rq++) {
      p_rqs[rq].pid = exec((void *) &endless_loop_wrap, argvAux, 0, "test", 1);
      

      
      if (p_rqs[rq].pid == -1) {
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

     // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < maxes; rq++) {
        action = GetUniform(100) % 2;
        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (kill(p_rqs[rq].pid) == -1) {
                return -1;
              }
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

           case 1:
             if (p_rqs[rq].state == RUNNING) { 
               if (block(p_rqs[rq].pid) == -1) {
                 return -1;
               }
              p_rqs[rq].state = BLOCKED;
             }
             break;
         }
      }
      
      // Randomly unblocks processes
      for (rq = 0; rq < maxes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (unblock(p_rqs[rq].pid) == -1) {
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
   return 0;
}
