#include <stdio.h>
#include <semaphore.h>
#include "monitor.h"

#define CHAIRS 6

/* ---- Custom Condition Variable ---- */
typedef struct {
    int   count;
    sem_t sem;
} CV;

/*
 * wait(cv): release the monitor mutex and suspend on the CV.
 * On wake-up, re-compete for the monitor (signal-and-continue: signaler
 * keeps the monitor; woken thread enters the entry queue).
 */
static void cv_wait(CV *cv, sem_t *mutex) {
    cv->count++;
    sem_post(mutex);
    sem_wait(&cv->sem);
    sem_wait(mutex);
}

/*
 * signal(cv): wake one thread blocked on the CV.
 * Signaler continues holding the monitor (signal-and-continue discipline).
 */
static void cv_signal(CV *cv) {
    if (cv->count > 0) {
        cv->count--;
        sem_post(&cv->sem);
    }
}

static int cv_count(CV *cv) {
    return cv->count;
}

/* ---- Monitor State ---- */
static sem_t entry_mutex;

static CV stylistAvailable;
static CV customerAvailable;

static int customer_count = 0;
static int stylist_ready  = 0;
static int being_cut      = 0;

/* Debug counters */
static int given_haircuts  = 0;
static int salon_full_count  = 0;
static int salon_empty_count = 0;

/* ---- Monitor Functions ---- */
void mon_init(void) {
    sem_init(&entry_mutex, 0, 1);
    sem_init(&stylistAvailable.sem,  0, 0);  stylistAvailable.count  = 0;
    sem_init(&customerAvailable.sem, 0, 0);  customerAvailable.count = 0;
}

/*
 * Called by the stylist at the top of its loop (before cutting hair).
 * Signals any customer waiting for the stylist, then waits for a customer
 * if the salon is empty.
 */
void mon_checkCustomer(void) {
    sem_wait(&entry_mutex);

    being_cut = 0;
    stylist_ready++;

    cv_signal(&stylistAvailable);

    if (customer_count == 0) {
        salon_empty_count++;
        printf("[Stylist] Salon empty, going to sleep (salon_empty=%d)\n",
               salon_empty_count);
        cv_wait(&customerAvailable, &entry_mutex);

        printf("[Stylist] Woke up, resuming inside monitor"
               " (signal-and-continue confirmed)\n");
    }

    customer_count--;
    being_cut = 1;
    given_haircuts++;

    sem_post(&entry_mutex);
}

/*
 * Called by a customer to try to enter the salon.
 * Returns 1 if the customer successfully got a seat (will be served),
 * or 0 if the salon was full (customer must go shopping and retry).
 */
int mon_checkStylist(void) {
    sem_wait(&entry_mutex);

    int status = 0;

    if (customer_count < CHAIRS) {
        customer_count++;
        cv_signal(&customerAvailable);

        if (stylist_ready == 0) {
            cv_wait(&stylistAvailable, &entry_mutex);

            printf("[Customer] Stylist ready, resuming inside monitor"
                   " (signal-and-continue confirmed)\n");
        }

        stylist_ready--;
        status = 1;
    } else {
        salon_full_count++;
        printf("[Customer] Salon full, going shopping (salon_full=%d)\n",
               salon_full_count);
    }

    sem_post(&entry_mutex);
    return status;
}

/*
 * Print the state of the waiting chairs plus key counters.
 * This function is INSIDE the monitor (only one thread at a time).
 * Format:  |0|1|1|0|0|0| => 2
 *          Given haircuts = X
 *          Salon full = Y times
 *          Salon empty = Z times
 */
void mon_debugPrint(void) {
    sem_wait(&entry_mutex);

    printf("|");
    int occupied = 0;
    for (int i = 0; i < CHAIRS; i++) {
        int chair_occupied = (i < customer_count) ? 1 : 0;
        occupied += chair_occupied;
        printf("%d|", chair_occupied);
    }
    printf(" => %d\n", occupied);
    printf("Given haircuts = %d\n",   given_haircuts);
    printf("Salon full = %d times\n", salon_full_count);
    printf("Salon empty = %d times\n", salon_empty_count);

    sem_post(&entry_mutex);
}

int mon_getHaircuts(void) {
    sem_wait(&entry_mutex);
    int h = given_haircuts;
    sem_post(&entry_mutex);
    return h;
}
