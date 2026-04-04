#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIRS 6
#define DELAY 1000000 // adjust this value
#define NUM_CUSTOMERS 10

sem_t mutex = 1;
sem_t stylist_s = 0;
sem_t customers = 0;
int waiting = 0;

void main ( void ) {
// create a specified number of customer threads
// and a stylist thread . Don ’ t f o r g e t t o j o i n t h r e a d s
}


// have not tested these two yet
void stylist (void) {
  int j;
  while (1) {
      sem_wait(&customers);
      sem_wait(&mutex);

      waiting = waiting - 1;

      sem_post(&stylist_s);
      sem_post(&mutex);

      for (j = 0; j < DELAY; j++);
  }
}

void customer(void) {
    int j;
    while (1) {
        sem_wait(&mutex);
        if (waiting < CHAIRS) {
            waiting = waiting + 1;

            sem_post(&customers);
            sem_post(&mutex);

            sem_wait(&stylist_s);
            break;
        } else {
            sem_post(&mutex);

            for (j = 0; j < DELAY; j++);
        }
    }

    return NULL;
}