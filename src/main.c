#include "ui/main_window.h"
#include "reactor/reactor.h"
#include <pthread.h>

int
main (int argc, char **argv) {
  bars = (struct bar*)malloc(sizeof(struct bar) * NUM_THREADS);
  /* Starting the bar threads */
  start_threads(bars);
  /* Starting the application */
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&bar_mutex);
  pthread_mutex_destroy(&write_mutex);
  pthread_cond_destroy(&unstable_state);
  pthread_exit(NULL);


  return 0;
}
