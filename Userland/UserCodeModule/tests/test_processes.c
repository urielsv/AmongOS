#include <stdio.h>
#include <tests.h>
#include <stdint.h>
#include <syscalls.h>
#include <definitions.h>

typedef struct p_rq {
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
  char *argv_aux[] = {0};

  if (argc != 1)
    return -1;

  if ((maxes = satoi(argv[0])) <= 0)
    return -1;

  p_rq p_rqs[maxes];


  while (1) {
    

     // create maxes processes
    for (rq = 0; rq < maxes; rq++) {
      p_rqs[rq].pid = exec((void *) &endless_loop_wrap, argv_aux, 0, "test", 1);
      
      if (p_rqs[rq].pid == -1) {
        printf("failed to create process\n");
        return -1;
      } else {
        printf(".\n");
        p_rqs[rq].state = running;
        alive++;
      }
    }

     // randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < maxes; rq++) {
        action = get_uniform(100) % 2;
        switch (action) {
          case 0:
            if (p_rqs[rq].state == running || p_rqs[rq].state == blocked) {
              if (kill(p_rqs[rq].pid) == -1) {
                printf("c\n");
                return -1;
              }
              printf("killing process %d\n", p_rqs[rq].pid);
              p_rqs[rq].state = killed;
              alive--;
            }
            break;

           case 1:
             if (p_rqs[rq].state == running) { 
               if (block(p_rqs[rq].pid) == -1) {
                  printf("d\n");
                 return -1;
               }
                printf("blocking process %d\n", p_rqs[rq].pid);
              p_rqs[rq].state = blocked;
             }
             break;
         }
      }
      
      // randomly unblocks processes
      for (rq = 0; rq < maxes; rq++)
        if (p_rqs[rq].state == blocked && get_uniform(100) % 2) {
          if (unblock(p_rqs[rq].pid) == -1) {
            printf("e\n");
            return -1;
          }
          printf("unblocking process %d\n", p_rqs[rq].pid);
          p_rqs[rq].state = running;
        }
    }
  }
   return 0;
}
