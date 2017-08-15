#ifndef _REACTOR_H_
#define _REACTOR_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 16
#define MAX_BALANCE_TIME  10 // max of 5 seconds to balance the system
#define MOVEMENT_TIME  2 // time used by the bar to move
#define CHANGE_DIRECTION  2 // time used by the bar to change direction
enum Direction {UP = 1, DOWN = 0}; 

double unstable_value;
int     thread_ids[4];
pthread_mutex_t bar_mutex;  //mutex for handling the movement of each bar
pthread_cond_t unstable_state; //condition variable 
pthread_attr_t attr;
bool unbalanced;
double  unstable_value, k_value;

struct bar{
  long id;
  enum Direction direction; 
  int cm; // cm representing the movement of the wire.
  double* k_value;
};


void*
move_bar(void *bar); 

void*
unstabilize(double value); 

void
start_threads(struct bar* bars);

double
getDeltaKValue(int val);

double
read_unstable_value();

void
fill_bar(struct bar* b, long id, double* k_value);

#endif
