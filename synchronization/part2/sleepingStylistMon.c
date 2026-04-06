#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "monitor.h"

#define DELAY        1000000   /* adjust this value */
#define NUM_CUSTOMERS 75

static void *stylist_func(void *arg) {
    int j;
    while (1) {
        mon_debugPrint();
        mon_checkCustomer();
        for (j = 0; j < DELAY; j++); /* cut hair */
        if (mon_getHaircuts() >= NUM_CUSTOMERS)
            break;
    }
    printf("[Stylist] All %d customers served. Done.\n", NUM_CUSTOMERS);
    return NULL;
}

static void *customer_func(void *arg) {
    int j;
    while (1) {
        mon_debugPrint();
        if (mon_checkStylist())
            break;
        for (j = 0; j < DELAY; j++); /* go shopping */
    }
    return NULL;
}

int main(void) {
    mon_init();

    pthread_t stylist_thread;
    pthread_t customer_threads[NUM_CUSTOMERS];

    pthread_create(&stylist_thread, NULL, stylist_func, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_create(&customer_threads[i], NULL, customer_func, NULL);

    pthread_join(stylist_thread, NULL);

    for (int i = 0; i < NUM_CUSTOMERS; i++)
        pthread_join(customer_threads[i], NULL);

    return 0;
}
