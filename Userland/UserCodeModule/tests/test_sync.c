// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <stdio.h>
#include <tests.h>
#include <syscalls.h>

#define sem_id 0
#define total_pair_processes 2

int64_t global; // shared memory

void slow_inc(int64_t *p, int64_t inc) {
    int64_t aux = *p;
    yield();
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    printf("\n_pid %d start (n=%s, inc=%s, sem=%s)\n", get_pid(), argv[0], argv[1], argv[2]);

    if (argc != 3) {
        printf("[error] invalid argument count: %d\n", argc);
        return -1;
    }

    if ((n = satoi(argv[0])) <= 0) {
        printf("[error] invalid n value: %s\n", argv[0]);
        return -1;
    }

    if ((inc = satoi(argv[1])) == 0) {
        printf("[error] invalid increment value: %s\n", argv[1]);
        return -1;
    }

    if ((use_sem = satoi(argv[2])) < 0) {
        printf("[error] invalid semaphore flag: %s\n", argv[2]);
        return -1;
    }

    printf("pid %d config: n=%d inc=%d sem=%d\n", get_pid(), n, inc, use_sem);

   // printf("use_sem value = %d\n",use_sem);
    if (use_sem==0) {
      //  printf("enter use_sem value = %d\n",use_sem);

        int aux = sem_open(sem_id, 1); 
        
        if ((aux ) == -1) {
            printf("pid %d sem: open failed\n", get_pid());
            return -1;
        }
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        printf("pid %d iter %d/%d\n", get_pid(), i+1, n);
        if (use_sem==0) {
            printf("pid %d sem: wait\n", get_pid());
            sem_wait(sem_id);
            printf("pid %d sem: got\n", get_pid());
        }
        printf("pre inc global: %d\n", global);
        slow_inc(&global, inc);
        printf("post inc global: %d\n", global);

        if (use_sem==0) {
            printf("pid %d sem: post\n", get_pid());
            sem_post(sem_id);
        }
    }

    if (use_sem==0) {
        sem_close(sem_id);
    }
    printf("pid %d end\n", get_pid());
    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) {
    uint64_t pids[2 * total_pair_processes];

    printf("\n=== test start (n=%s, sem=%s) ===\n", argv[0], argv[1]);

    if (argc != 2) {
        printf("[error] invalid args: %d\n", argc);
        return -1;
    }

    char *argv_dec[] = {argv[0], "-1", argv[1], NULL};
    char *argv_inc[] = {argv[0], "1", argv[1], NULL};

    global = 0;

    uint64_t i;
    for (i = 0; i < total_pair_processes; i++) {
        printf("\n_creating pair %d:\n", i+1);
        pids[i] = exec((void *)&my_process_inc, argv_dec, 3, "my_process_dec", 1);
        printf("\n- dec pid: %d, global:%d \n", pids[i], global);
        
        pids[i + total_pair_processes] = exec((void *)&my_process_inc, argv_inc, 3, "my_process_inc", 1);
        printf("\n- inc pid: %d\n, global: %d", pids[i + total_pair_processes], global);
    }

    printf("\n_waiting for processes\n");

    for (i = 0; i < total_pair_processes; i++) {
        printf("wait dec : %d\n", pids[i]);
        waitpid(pids[i]);
        printf("done\n");

        printf("wait inc : %d\n", pids[i + total_pair_processes]);
        waitpid(pids[i + total_pair_processes]);
        printf("done\n");
    }
    sleep(5000);

    //waitpid(pid); todo: handle waitpid(-1) to wpid any child
    printf("\n=== test end ===\n");
    printf("final global: %d\n", global);
    return 0;
}
