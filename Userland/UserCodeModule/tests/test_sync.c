#include <stdint.h>
#include <stdio.h>
#include <tests.h>
#include <syscalls.h>

#define SEM_ID 1
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
    printf("[DEBUG] Reading global value: %d, will add: %d\n", *p, inc);
    uint64_t aux = *p;
    printf("[DEBUG] Before yield, aux = %d\n", aux);
    yield(); // This makes the race condition highly probable
    aux += inc;
    printf("[DEBUG] After yield, aux = %d\n", aux);
    *p = aux;
    printf("[DEBUG] Updated global to: %d\n", *p);
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
    uint64_t n;
    int8_t inc;
    int8_t use_sem;

    printf("[DEBUG] Process started with PID: %d\n", get_pid());
    printf("[DEBUG] Args received: n=%s, inc=%s, use_sem=%s\n", argv[0], argv[1], argv[2]);

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

    printf("[DEBUG] Process will do %d iterations with inc=%d (sem=%d)\n", n, inc, use_sem);

    if (use_sem) {
        printf("[DEBUG] Opening semaphore %d\n", SEM_ID);
        if (!sem_open(SEM_ID, 1)) {
            printf("test_sync: ERROR opening semaphore\n");
            return -1;
        }
        printf("[DEBUG] Semaphore opened successfully\n");
    }

    uint64_t i;
    for (i = 0; i < n; i++) {
        printf("[DEBUG] PID %d: Iteration %d/%d\n", get_pid(), i+1, n);
        
        if (use_sem) {
            printf("[DEBUG] PID %d: Waiting for semaphore\n", get_pid());
            sem_wait(SEM_ID);
            printf("[DEBUG] PID %d: Got semaphore\n", get_pid());
        }

        slowInc(&global, inc);

        if (use_sem) {
            printf("[DEBUG] PID %d: Releasing semaphore\n", get_pid());
            sem_post(SEM_ID);
            printf("[DEBUG] PID %d: Released semaphore\n", get_pid());
        }
    }

    if (use_sem) {
        printf("[DEBUG] PID %d: Closing semaphore\n", get_pid());
        sem_close(SEM_ID);
    }

    printf("[DEBUG] PID %d: Process completed successfully\n", get_pid());
    return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) {
    uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

    printf("[DEBUG] Starting test_sync with args: n=%s, use_sem=%s\n", argv[0], argv[1]);

    if (argc != 2) {
        printf("[ERROR] test_sync: Invalid argument count: %d\n", argc);
        return -1;
    }

    char *argvDec[] = {argv[0], "-1", argv[1], NULL};
    char *argvInc[] = {argv[0], "1", argv[1], NULL};

    printf("[DEBUG] Initial global value: %d\n", global);
    global = 0;

    uint64_t i;
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        printf("[DEBUG] Creating process pair %d\n", i+1);  
        
        pids[i] = exec((void *)&my_process_inc, argvDec, 3, "my_process_dec", 1);
        printf("[DEBUG] Created decrementer process with PID: %d\n", pids[i]);
        
        pids[i + TOTAL_PAIR_PROCESSES] = exec((void *)&my_process_inc, argvInc, 3, "my_process_inc", 1);
        printf("[DEBUG] Created incrementer process with PID: %d\n", pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("[DEBUG] Waiting for all processes to complete\n");

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        printf("[DEBUG] Waiting for process %d\n", pids[i]);
        my_wait(pids[i]);
        printf("[DEBUG] Process %d completed\n", pids[i]);

        printf("[DEBUG] Waiting for process %d\n", pids[i + TOTAL_PAIR_PROCESSES]);
        my_wait(pids[i + TOTAL_PAIR_PROCESSES]);
        printf("[DEBUG] Process %d completed\n", pids[i + TOTAL_PAIR_PROCESSES]);
    }

    printf("[DEBUG] All processes completed\n");
    printf("Final value: %d\n", global);
    return 0;
}