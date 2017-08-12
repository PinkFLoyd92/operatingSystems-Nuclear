#ifndef _REACTOR_H_
#define _REACTOR_H_
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS  4
enum Direction {UP = 1, DOWN = 0}; 

double unstable_value;
int     thread_ids[4];
pthread_mutex_t bar_mutex;  //mutex for handling the movement of each bar
pthread_cond_t unstable_state; //condition variable 
pthread_attr_t attr;
bool unbalanced;

struct bar{
  int id;
  enum Direction direction; 
  int value; // cm representing the movement of the wire.
};


void*
move_bar(void *t); 

void*
unstabilize(double value); 

void
start_threads();
#endif
