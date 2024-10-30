#include <stdint.h>
#include <stdio.h>
#include <tests.h>
#include <syscalls.h>

#define SEM_ID 0
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
    int64_t aux = *p;
    yield();
    aux += inc;
    *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    printf("\nPID %d START (n=%s, inc=%s, sem=%s)\n", get_pid(), argv[0], argv[1], argv[2]);

    if (argc != 3) {
        printf("[ERROR] Invalid argument count: %d\n", argc);
        return -1;
    }

    if ((n = satoi(argv[0])) <= 0) {
        printf("[ERROR] Invalid n value: %s\n", argv[0]);
        return -1;
    }

    if ((inc = satoi(argv[1])) == 0) {
        printf("[ERROR] Invalid increment value: %s\n", argv[1]);
        return -1;
    }

    if ((use_sem = satoi(argv[2])) < 0) {
        printf("[ERROR] Invalid semaphore flag: %s\n", argv[2]);
        return -1;
    }

    printf("PID %d CONFIG: n=%d inc=%d sem=%d\n", get_pid(), n, inc, use_sem);

   // printf("use_sem value = %d\n",use_sem);
    if (use_sem==0) {
      //  printf("enter use_sem value = %d\n",use_sem);

        int aux = sem_open(SEM_ID, 1); 
        
        if ((aux ) == -1) {
            printf("PID %d SEM: Open failed\n", get_pid());
            return -1;
        }
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        printf("PID %d ITER %d/%d\n", get_pid(), i+1, n);
        if (use_sem==0) {
            printf("PID %d SEM: Wait\n", get_pid());
            sem_wait(SEM_ID);
            printf("PID %d SEM: Got\n", get_pid());
        }
        printf("PRE INC GLOBAL: %d\n", global);
        slowInc(&global, inc);
        printf("POST INC GLOBAL: %d\n", global);

        if (use_sem==0) {
            printf("PID %d SEM: Post\n", get_pid());
            sem_post(SEM_ID);
        }
    }

    if (use_sem) {
        printf("PID %d SEM: Closing\n", get_pid());
        sem_close(SEM_ID);
    }

    printf("PID %d END\n", get_pid());
    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) {
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    printf("\n=== TEST START (n=%s, sem=%s) ===\n", argv[0], argv[1]);

    if (argc != 2) {
        printf("[ERROR] Invalid args: %d\n", argc);
        return -1;
    }

    char *argvDec[] = {argv[0], "-1", argv[1], NULL};
    char *argvInc[] = {argv[0], "1", argv[1], NULL};

    //printf("Initial global: %d\n", global);
    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        printf("\nCreating pair %d:\n", i+1);
        pids[i] = exec((void *)&my_process_inc, argvDec, 3, "my_process_dec", 1);
        printf("\n- DEC PID: %d, GLOBAL:%d \n", pids[i], global);
        
        pids[i + TOTAL_PAIR_PROCESSES] = exec((void *)&my_process_inc, argvInc, 3, "my_process_inc", 1);
        printf("\n- INC PID: %d\n, GLOBAL: %d", pids[i + TOTAL_PAIR_PROCESSES], global);
    }

    printf("\nWaiting for processes\n");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        printf("Wait DEC %d: ", pids[i]);
        my_wait(pids[i]);
        printf("Done\n");

        printf("Wait INC %d: ", pids[i + TOTAL_PAIR_PROCESSES]);
        my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
        printf("Done\n");
    }

    sleep(30000000000000000);
    printf("\n=== TEST END ===\n");
    printf("Final global: %d\n", global);
    return 0;
}