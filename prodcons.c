#include <stdio.h>

#define MAX 5

int buffer[MAX];
int top = -1;
int itemCount = 0;

// ─── Semaphore Structure ───────────────────────────────────
typedef struct {
    int value;
} Semaphore;

Semaphore empty = {MAX};  // tracks empty slots  → starts at MAX
Semaphore full  = {0};    // tracks filled slots → starts at 0
Semaphore mutex = {1};    // binary semaphore for mutual exclusion

// ─── Wait (P operation) ────────────────────────────────────
void wait(Semaphore *s) {
    s->value--;
}

// ─── Signal (V operation) ──────────────────────────────────
void signal(Semaphore *s) {
    s->value++;
}

// ─── Producer ─────────────────────────────────────────────
void produce() {
    if (empty.value <= 0) {
        printf("Buffer is full!\n");
        return;
    }

    wait(&empty);   // one less empty slot
    wait(&mutex);   // enter critical section

    itemCount++;
    top++;
    buffer[top] = itemCount;
    printf("Producer has produced: Item %d\n", itemCount);

    signal(&mutex); // leave critical section
    signal(&full);  // one more filled slot
}

// ─── Consumer ─────────────────────────────────────────────
void consume() {
    if (full.value <= 0) {
        printf("Buffer is empty!\n");
        return;
    }

    wait(&full);    // one less filled slot
    wait(&mutex);   // enter critical section

    printf("Consumer has consumed: Item %d\n", buffer[top]);
    top--;

    signal(&mutex); // leave critical section
    signal(&empty); // one more empty slot
}

// ─── Main ─────────────────────────────────────────────────
int main() {
    int choice;
    while (1) {
        printf("Enter 1.Producer 2.Consumer 3.Exit\n");
        printf("Enter your choice:\n");
        scanf("%d", &choice);

        if      (choice == 1) produce();
        else if (choice == 2) consume();
        else if (choice == 3) break;
        else                  printf("Invalid choice!\n");
    }
    return 0;
}