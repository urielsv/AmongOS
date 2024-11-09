#include <philosophers.h>
#include <syscalls.h>
#include <stdio.h>
#include <random.h>

#define MAX_PHILOSOPHERS 12
#define MIN_PHILOSOPHERS 5
#define SEM_ID ((1<<6) - 1) //start at end
#define philo_sem(i) (SEM_ID - (i) - 1)
#define left(i) (((i) + philosophers_count - 1) % philosophers_count)	
#define right(i) (((i) + 1) % philosophers_count)
#define MAX_THINKING_TIME 10000
#define MIN_THINKING_TIME 1000

typedef enum {
    NONE = 0,
    EATING,
    HUNGRY,
    THINKING
} philo_state;


typedef struct {
    philo_state state;
    int philo_pid;
} philo_t;

static char * philo_names[MAX_PHILOSOPHERS] = {
    "Yasuo",
    "Zed",
    "Riven",
    "Darius",
    "Garen",
    "Jinx",
    "Jhin",
    "Katarina",
    "Akali",
    "Ahri",
    "Lux",
    "Ezreal"
};

static philo_t philosophers[MAX_PHILOSOPHERS];
static int philosophers_count = 0;

static int add_philo(int idx);
static int remove_philo(int idx);
static void philosopher(int argc, char **argv);
static void take_forks(int i);
static void put_forks(int i);
static void test(int i);
static void print_state();
static void think();
static void kill_philos();



int run_philosophers(int argc, char* argv[]){
    
    if(sem_open(SEM_ID, 1) < 0){
        printf("Error opening semaphore\n");
        return -1;
    }
    int start_philosophers = 0;

    if(argc == 0){
        int seed = atoi(time());
        printf("seed: %d\n", seed);
        start_philosophers = rand(seed, MAX_PHILOSOPHERS - MIN_PHILOSOPHERS) + MIN_PHILOSOPHERS;
    }else{
        start_philosophers = atoi(argv[0]);
    
        if(philosophers_count < MIN_PHILOSOPHERS || philosophers_count > MAX_PHILOSOPHERS){
            printf("Invalid number of philosophers. Must be between %d and %d\n", MIN_PHILOSOPHERS, MAX_PHILOSOPHERS);
            return -1;
        }
    }

    printf("Starting the dilemma with %d philosophers\n", start_philosophers);

    for(int i = 0; i < start_philosophers; i++){
        philosophers[i].state = NONE;
        philosophers[i].philo_pid = -1;
    }

    for (int i = 0; i < start_philosophers; i++){
        add_philo(i);
    }

    char c = '\0';
    while((c = getchar()) != 'q'){
        switch(c){
            case 'a':
                if(philosophers_count < MAX_PHILOSOPHERS){
                    add_philo(philosophers_count);
                }else{
                    printf("The table is full of philosophers\n");
                }
                break;
            case 'r':
                if(philosophers_count > MIN_PHILOSOPHERS){
                    remove_philo(philosophers_count - 1);
                    
                }else{
                    printf("The table should have at least %d philosophers\n", MIN_PHILOSOPHERS);
                }
                break;
        }
    }
    kill_philos();
    sem_close(SEM_ID);
    return 0;
}

static int add_philo(int idx){
    printf("The philosopher %s has joined the table\n", philo_names[idx]);
    sem_wait(SEM_ID);

    if(sem_open(philo_sem(idx), 1) < 0){
        printf("Error opening semaphore\n");
        return -1;
    }

    char philo_idx_buff[3] = {0};
    itoa(idx, philo_idx_buff);
    char *argv[] = {philo_idx_buff, NULL};
    
    philosophers[idx].philo_pid = exec((void *)philosopher, argv, 1, "philosopher", 1);
    printf("PID: %d\n", philosophers[idx].philo_pid);
    if(philosophers[idx].philo_pid != -1)
            philosophers_count++;
    
    sem_post(SEM_ID);
    return -1 * !(philosophers[idx].philo_pid + 1);
}

static int remove_philo(int idx){
    printf("The philosopher %d %s has left the table\n", idx, philo_names[idx]);
    sem_wait(SEM_ID);
    while(philosophers[left(idx)].state == EATING && philosophers[right(idx)].state == EATING){
        sem_post(SEM_ID);
        sem_wait(philo_sem(idx));
        sem_wait(SEM_ID);
    }
    kill(philosophers[idx].philo_pid);
    waitpid(philosophers[idx].philo_pid);
    sem_close(philo_sem(idx));
    philosophers[idx].philo_pid = -1;
    philosophers[idx].state = NONE;
    philosophers_count--;

    sem_post(SEM_ID);
    
    return 0;
}

static void philosopher(int argc, char *argv[]){
    int i = atoi(argv[0]);
    philosophers[i].state = THINKING;
    while(1){
        think();
        take_forks(i);
        print_state();
        think();
        put_forks(i);
    }
}

static void take_forks(int i){
    sem_wait(SEM_ID);
    philosophers[i].state = HUNGRY;
    test(i);
    sem_post(SEM_ID);
    sem_wait(philo_sem(i));
}

static void put_forks(int i){
    sem_wait(SEM_ID);
    philosophers[i].state = THINKING;
    test(left(i));
    test(right(i));
    sem_post(SEM_ID);
}

static void test(int i){
    if(philosophers[i].state == HUNGRY && philosophers[left(i)].state != EATING && philosophers[right(i)].state != EATING){
        philosophers[i].state = EATING;
        sem_post(philo_sem(i));

    }
}

static void print_state(){
    for(int i = 0; i < philosophers_count; i++){
        printf("%s ", philosophers[i].state == EATING ? "E" : ".");
    }
    printf("\n");
}

static void think(){
    int seed = atoi(time());
    sleep(rand(seed, MAX_THINKING_TIME - MIN_THINKING_TIME) + MIN_THINKING_TIME);
}

static void kill_philos(){
    for(int i=0; i < philosophers_count - 1; i++){
        remove_philo(i);
    }
}