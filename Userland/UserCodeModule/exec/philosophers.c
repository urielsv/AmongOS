#include <philosophers.h>
#include <syscalls.h>
#include <stdio.h>
#include <random.h>

#define max_philosophers 12
#define min_philosophers 5
#define sem_id ((1<<6) - 1) //start at end
#define philo_sem(i) (sem_id - (i) - 1)
#define left(i) (((i) + philosophers_count - 1) % philosophers_count)	
#define right(i) (((i) + 1) % philosophers_count)
#define max_thinking_time 10000
#define min_thinking_time 1000

typedef enum {
    none = 0,
    eating,
    hungry,
    thinking
} philo_state;


typedef struct {
    philo_state state;
    int philo_pid;
} philo_t;

static char * philo_names[max_philosophers] = {
    "yasuo",
    "zed",
    "riven",
    "darius",
    "garen",
    "jinx",
    "jhin",
    "katarina",
    "akali",
    "ahri",
    "lux",
    "ezreal"
};

static philo_t philosophers[max_philosophers];
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
     philosophers_count = 0;
    if(sem_open(sem_id, 1) < 0){
        printf("error opening semaphore\n");
        return -1;
    }
    int start_philosophers = 0;

    if(argc == 0){
        int seed = atoi(time());
        printf("seed: %d\n", seed);
        start_philosophers = rand(seed, max_philosophers - min_philosophers) + min_philosophers;
    }else{
        start_philosophers = atoi(argv[0]);
    
        if(philosophers_count < min_philosophers || philosophers_count > max_philosophers){
            printf("invalid number of philosophers. must be between %d and %d\n", min_philosophers, max_philosophers);
            return -1;
        }
    }

    start_philosophers = 5;
    printf("starting the dilemma with %d philosophers\n", start_philosophers);

    for(int i = 0; i < start_philosophers; i++){
        philosophers[i].state = none;
        philosophers[i].philo_pid = -1;
    }

    for (int i = 0; i < start_philosophers; i++){
        add_philo(i);
    }

    char c = '\0';
    while((c = getchar()) != 'q'){
        switch(c){
            case 'a':
                if(philosophers_count < max_philosophers){
                    add_philo(philosophers_count);
                }else{
                    printf("the table is full of philosophers\n");
                }
                break;
            case 'r':
                if(philosophers_count > min_philosophers){
                    remove_philo(philosophers_count - 1);
                    
                }else{
                    printf("the table should have at least %d philosophers\n", min_philosophers);
                }
                break;
        }
    }
    kill_philos();
    sem_close(sem_id);
    return 0;
}

static int add_philo(int idx){
    printf("the philosopher %s has joined the table\n", philo_names[idx]);
    sem_wait(sem_id);

    if(sem_open(philo_sem(idx), 1) < 0){
        printf("error opening semaphore\n");
        return -1;
    }

    char philo_idx_buff[3] = {0};
    itoa(idx, philo_idx_buff);
    char *argv[] = {philo_idx_buff, NULL};
    
    philosophers[idx].philo_pid = exec((void *)philosopher, argv, 1, "philosopher", 1);
    if(philosophers[idx].philo_pid != -1)
            philosophers_count++;
    
    sem_post(sem_id);
    return -1 * !(philosophers[idx].philo_pid + 1);
}

static int remove_philo(int idx){
    printf("the philosopher %d %s has left the table\n", idx, philo_names[idx]);
    sem_wait(sem_id);
    while(philosophers[left(idx)].state == eating && philosophers[right(idx)].state == eating){
        sem_post(sem_id);
        sem_wait(philo_sem(idx));
        sem_wait(sem_id);
    }
    kill(philosophers[idx].philo_pid);
    sem_close(philo_sem(idx));
    philosophers[idx].philo_pid = -1;
    philosophers[idx].state = none;
    philosophers_count--;

    sem_post(sem_id);
    
    return 0;
}

static void philosopher(int argc, char *argv[]){
    int i = atoi(argv[0]);
    philosophers[i].state = thinking;
    while(1){
        think();
        take_forks(i);
        print_state();
        think();
        put_forks(i);
    }
}

static void take_forks(int i){
    sem_wait(sem_id);
    philosophers[i].state = hungry;
    test(i);
    sem_post(sem_id);
    sem_wait(philo_sem(i));
}

static void put_forks(int i){
    sem_wait(sem_id);
    philosophers[i].state = thinking;
    test(left(i));
    test(right(i));
    sem_post(sem_id);
}

static void test(int i){
    if(philosophers[i].state == hungry && philosophers[left(i)].state != eating && philosophers[right(i)].state != eating){
        philosophers[i].state = eating;
        sem_post(philo_sem(i));

    }
}

static void print_state(){
    for(int i = 0; i < philosophers_count; i++){
        printf("%s ", philosophers[i].state == eating ? "e" : "."); 
    }
    printf("\n");
}

static void think(){
    sleep(1000);
    //int seed = atoi(time());
    //sleep(rand(seed, max_thinking_time - min_thinking_time) + min_thinking_time);
}

static void kill_philos(){
    for(int i=0; i < philosophers_count - 1; i++){
        remove_philo(i);
    }
}