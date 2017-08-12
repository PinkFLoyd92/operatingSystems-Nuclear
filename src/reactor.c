#include "reactor.h"

void
start_threads(){
  int i;
  long t1=1, t2=2, t3=3, t4=4; //internal thread id
  pthread_t threads[NUM_THREADS];
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_cond_init(&unstable_state, NULL);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&threads[0], &attr, move_bar, (void *)t1);
  pthread_create(&threads[1], &attr, move_bar, (void *)t2);
  pthread_create(&threads[2], &attr, move_bar, (void *)t3);
  pthread_create(&threads[3], &attr, move_bar, (void *)t4);


  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}


void*
move_bar(void *t){
  long my_id = (long)t;
  while(true){
    pthread_mutex_lock(&bar_mutex);
    while(!unbalanced)
      pthread_cond_wait(&unstable_state, &bar_mutex);
    printf("Moviendo barra usando el hilo: %ld\n", my_id);
    //modificamos el valor de la barra
    //revisamos si debemos hacer el movimiento hacia arriba o abajo
    pthread_mutex_unlock(&bar_mutex);
  }
  pthread_exit(NULL);
}

void*
unstabilize(double value){
     pthread_mutex_lock(&bar_mutex);
     /* read new value  and change variable unstable_value*/
     // change balanced to false
     pthread_cond_signal(&unstable_state);
     pthread_mutex_unlock(&bar_mutex);

}
