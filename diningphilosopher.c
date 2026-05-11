#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MAX_PHILOSOPHERS 10

typedef struct {
    int id;
    int is_hungry;
    int strategy;
} Philosopher;

pthread_mutex_t lock;
int num_philosophers;
int num_hungry;
int num_eating = 0;
int eating_strategy;
int *hungry_list;

void* philosopher_thread(void* arg) {
    Philosopher* ph = (Philosopher*)arg;
    
    if (!ph->is_hungry) {
        return NULL;
    }
    
    int can_eat = 0;
    
    while (!can_eat) {
        pthread_mutex_lock(&lock);
        
        if (eating_strategy == 1) {
            // Only one can eat at a time
            if (num_eating == 0) {
                printf("P %d is granted to eat\n", ph->id);
                fflush(stdout);
                num_eating++;
                can_eat = 1;
                pthread_mutex_unlock(&lock);
                
                usleep(500000);  // 0.5 seconds
                
                pthread_mutex_lock(&lock);
                printf("P %d has finished eating\n", ph->id);
                fflush(stdout);
                num_eating--;
                pthread_mutex_unlock(&lock);
            } else {
                printf("P %d is waiting\n", ph->id);
                fflush(stdout);
                pthread_mutex_unlock(&lock);
                usleep(100000);  // 0.1 seconds
            }
        } 
        else if (eating_strategy == 2) {
            // Two can eat at a time
            if (num_eating < 2) {
                printf("P %d is granted to eat\n", ph->id);
                fflush(stdout);
                num_eating++;
                can_eat = 1;
                pthread_mutex_unlock(&lock);
                
                usleep(500000);  // 0.5 seconds
                
                pthread_mutex_lock(&lock);
                printf("P %d has finished eating\n", ph->id);
                fflush(stdout);
                num_eating--;
                pthread_mutex_unlock(&lock);
            } else {
                printf("P %d is waiting\n", ph->id);
                fflush(stdout);
                pthread_mutex_unlock(&lock);
                usleep(100000);  // 0.1 seconds
            }
        }
        else {
            pthread_mutex_unlock(&lock);
        }
    }
    
    return NULL;
}

int main() {
    int i, choice;
    pthread_t threads[MAX_PHILOSOPHERS];
    
    printf("Enter the total number of philosophers: ");
    scanf("%d", &num_philosophers);
    
    printf("How many are hungry: ");
    scanf("%d", &num_hungry);
    
    hungry_list = (int*)malloc(num_hungry * sizeof(int));
    
    Philosopher philosophers[MAX_PHILOSOPHERS];
    
    // Initialize all philosophers as not hungry
    for (i = 0; i < num_philosophers; i++) {
        philosophers[i].id = i + 1;
        philosophers[i].is_hungry = 0;
    }
    
    // Get positions of hungry philosophers
    for (i = 0; i < num_hungry; i++) {
        printf("Enter philosopher %d position (1 to %d): ", i + 1, num_philosophers);
        scanf("%d", &hungry_list[i]);
        philosophers[hungry_list[i] - 1].is_hungry = 1;
    }
    
    // Menu
    printf("\n1. One can eat at a time\n");
    printf("2. Two can eat at a time\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    if (choice == 3) {
        return 0;
    }
    
    eating_strategy = choice;
    pthread_mutex_init(&lock, NULL);
    
    // Create threads for hungry philosophers only
    int thread_count = 0;
    for (i = 0; i < num_philosophers; i++) {
        if (philosophers[i].is_hungry) {
            philosophers[i].strategy = eating_strategy;
            pthread_create(&threads[thread_count], NULL, philosopher_thread, 
                         (void*)&philosophers[i]);
            thread_count++;
        }
    }
    
    // Wait for all threads to complete
    for (i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("\n1. One can eat at a time\n");
    printf("2. Two can eat at a time\n");
    printf("3. Exit\n");
    
    pthread_mutex_destroy(&lock);
    free(hungry_list);
    
    return 0;
}