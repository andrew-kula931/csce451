#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define CHAIRS 6
#define DELAY 10000000
#define NUM_CUSTOMERS 75
sem_t mutex, stylists, customers;
int waiting = 0;
int total_customers = 0;

void stylist(void) {
  while (1) {
    sem_wait(&customers);
    sem_wait(&mutex);
    waiting = waiting - 1;
    sem_post(&stylists);
    sem_post(&mutex);
    for (int j = 0; j < DELAY; j++); // cut hair
    printf("Haircut finished\n");

    sem_wait(&mutex);
    int done = (total_customers == NUM_CUSTOMERS && waiting == 0);
    sem_post(&mutex);
    if (done) break;
  }
}

void customer(void) {
  // Each customer should appear randomly
  int random_wait_time = rand() % 200000000 + 100000000;
  while (1) {
    for (int j = 0; j < random_wait_time; j++); // offset arrival time
    sem_wait(&mutex);
    if (waiting < CHAIRS){
      waiting = waiting + 1;
      total_customers = total_customers + 1;
      // Prints have to be within the critcal section to ensure consistent data
      printf("Customer %d takes a seat: ", total_customers);
      printf("Currently waiting: %d\n", waiting);
      sem_post(&customers);
      sem_post(&mutex);
      sem_wait(&stylists);
      break;
    } else {
      sem_post(&mutex);
      for (int j = 0; j < random_wait_time; j++); // go shopping
    }
  }
}

int main(void) {
  sem_init(&mutex, 0, 1);
  sem_init(&stylists, 0, 0);
  sem_init(&customers, 0, 0);
  pthread_t stylist_thread;
  pthread_t customer_threads[NUM_CUSTOMERS];
  pthread_create(&stylist_thread, NULL, (void *)stylist, NULL);
  for (int i = 0; i < NUM_CUSTOMERS; i++)
    pthread_create(&customer_threads[i], NULL, (void *)customer, NULL);
  pthread_join(stylist_thread, NULL);
  for (int i = 0; i < NUM_CUSTOMERS; i++)
    pthread_join(customer_threads[i], NULL);
  return 0;
}