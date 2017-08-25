#ifndef _REACTOR_H_
#define _REACTOR_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include "../socket/socket.h"

#define NUM_THREADS 16
#define MAX_BALANCE_TIME  20 // max of 4 seconds to balance the system
#define MOVEMENT_TIME  5 // time used by the bar to move
#define CHANGE_DIRECTION  10 // time used by the bar to change direction
enum Direction {UP = 1, DOWN = 0}; 

pthread_mutex_t bar_mutex;
pthread_mutex_t readK_mutex;
sem_t write_mutex;
pthread_cond_t unstable_state;
pthread_attr_t attr;
bool unbalanced, system_off;
double unstable_value, k_value, k_total;
struct bar* bars;

// usados para el movimiento a pares.
pthread_cond_t turn_cond[NUM_THREADS];
pthread_mutex_t mutex_cond[NUM_THREADS];
bool turn[NUM_THREADS];


struct bar{
  long id;
  enum Direction direction; 
  int cm; // cm representing the movement of the wire.
  double deltak;
};


void*
move_bar(void *bar); 

void*
count_time(void *time);

void
start_threads(struct bar* bars);

double getKValue(int val);

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

void* ask_value(void* bars);

void* count_unstable(void* bars);

void
init_turn_conds();

bool
areTurnsOff();
#endif
