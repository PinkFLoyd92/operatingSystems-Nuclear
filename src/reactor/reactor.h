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
#define MAX_BALANCE_TIME  8 // max of 4 seconds to balance the system
#define MOVEMENT_TIME  2 // time used by the bar to move
#define CHANGE_DIRECTION  2 // time used by the bar to change direction
enum Direction {UP = 1, DOWN = 0}; 

pthread_mutex_t bar_mutex;  //mutex for handling the movement of each bar
pthread_mutex_t read_unstable_mutex;  //mutex for handling the movement of each bar
sem_t write_mutex;  //mutex for handling the movement of each bar
pthread_cond_t unstable_state, ktotal_cond;
pthread_attr_t attr;
bool unbalanced, system_off, flag_ktotal;
double unstable_value, k_value, k_total;
struct bar* bars;

struct bar{
  long id;
  enum Direction direction; 
  int cm; // cm representing the movement of the wire.
  double deltak;
};

pthread_cond_t cond_bar[NUM_THREADS]; // currently we have 16 threads
bool turn[NUM_THREADS];


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

void* ask_value(void* bars);

void* count_unstable(void* bars);

bool
is_turn_available();

int
get_opposite_bar(int num);

int
select_first_bar_available();

#endif
