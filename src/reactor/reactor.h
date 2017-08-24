#ifndef _REACTOR_H_
#define _REACTOR_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include "../socket/socket.h"

typedef struct prio_lock {
    pthread_cond_t cond;
    pthread_mutex_t cv_mutex; /* Condition variable mutex */
    pthread_mutex_t cs_mutex; /* Critical section mutex */
    unsigned long high_waiters;
} prio_lock_t;

#define PRIO_LOCK_INITIALIZER { PTHREAD_COND_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }
#define NUM_THREADS 4
#define MAX_BALANCE_TIME  10 // max of 5 seconds to balance the system
#define MOVEMENT_TIME  1 // time used by the bar to move
#define CHANGE_DIRECTION  1 // time used by the bar to change direction
enum Direction {UP = 1, DOWN = 0}; 

prio_lock_t* prio_mutex;
double timeUnstabilized;
pthread_mutex_t bar_mutex;  //mutex for handling the movement of each bar
pthread_mutex_t write_mutex;  //mutex for handling the movement of each bar
pthread_cond_t unstable_state, writer_state; //condition variable 
pthread_attr_t attr;
bool unbalanced, isWriting;
double unstable_value, k_value, k_total;
struct bar* bars;

struct bar{
  long id;
  enum Direction direction; 
  int cm; // cm representing the movement of the wire.
};


void*
move_bar(void *bar); 

void*
count_time(void *time);

void
start_threads(struct bar* bars);

double getDeltaKValue(int val);

double
read_unstable_value();

void
fill_bar(struct bar* b, long id);

void
init_variables();

void*
check_stable(void* bars);

void
print_bars(struct bar* bars);

void
prio_lock_low(prio_lock_t *prio_lock);

void
prio_unlock_low(prio_lock_t *prio_lock);

void prio_lock_high(prio_lock_t *prio_lock);

void prio_unlock_high(prio_lock_t *prio_lock);

#endif
