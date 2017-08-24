#include "reactor.h"

//TODO: crear nuevo hilo para notificar el tiempo que lleva desestabilizado el reactor
void
start_threads(struct bar* bars){
  int i;
  pthread_t threads[NUM_THREADS + 2];
  bars = (struct bar*)malloc(sizeof(struct bar) * NUM_THREADS);
  init_variables();
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_mutex_init(&write_mutex, NULL);
  pthread_cond_init(&unstable_state, NULL);

  for (i = 1; i < NUM_THREADS + 1; i++) {
    fill_bar(&bars[i - 1], i);
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  // creating the 4 threads that handle each bar
  for (i = 0; i < NUM_THREADS; i++) {
  pthread_create(&threads[i], &attr, move_bar, (void *)&bars[i]);
  }
  //thread used to check if system is stable and change its k_total
  pthread_create(&threads[NUM_THREADS], &attr, check_stable, (void *)bars);
  pthread_create(&threads[NUM_THREADS + 1], &attr, ask_value, (void *)bars);

  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS + 1; i++) {
    pthread_join(threads[i], NULL);
  }
  
}

void*
ask_value(void* bars){
  while(true){
    printf("\nREADING VALUE FROM USER: ");
    unstable_value = read_unstable_value();
    printf("\nValor leido: %lf\n", unstable_value);
  }
}

void*
check_stable(void* bars){
  struct bar* wires = (struct bar*) bars ;
  while (true) {
    pthread_mutex_lock(&write_mutex);
    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += getDeltaKValue(wires[i].cm);
    }

    char str[25];
    sprintf(str,"deltak=%lf", k_total);
    doPost("deltak",str);
    
    k_total = k_value + k_total;

    sprintf(str,"kparcial=%lf",k_value);
    doPost("kparcial",str);
    sprintf(str,"ktotal=%lf", k_total);
    doPost("ktotal",str);
    if((double)k_total != (double)1.0){
       unbalanced = true;
       pthread_cond_signal(&unstable_state);
    }else{
      unbalanced = false;
    }
    pthread_mutex_unlock(&write_mutex);
    sleep(1);
  }
}

double
read_unstable_value(){
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &unstable_value);
  k_value = k_value + unstable_value; // de una vez calculamos el valor de k
  return k_value;
}

void*
move_bar(void *bar){
  struct bar* b = (struct bar*) bar ;
  clock_t start = clock();
  bool changed_direction = false;
  while(true){
    pthread_mutex_lock(&bar_mutex);
    while(unbalanced == false)
      pthread_cond_wait(&unstable_state, &bar_mutex);
    pthread_mutex_lock(&write_mutex);
    if (k_total < 1 && b->cm <=20) {
      b->cm = b->cm + 10; //Usando este valor calculamos el deltak
      /* printf("\nThread yendo hacia abajo\n"); */
      if(b->direction == UP){
        b->direction = DOWN;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }else if(k_total > 1 && b->cm >=-20){
      b->cm = b->cm - 10; //Usando este valor calculamos el deltak
      /* printf("\nThread yendo hacia arriba\n"); */
      if(b->direction == DOWN){
        b->direction = UP;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }

    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += getDeltaKValue(bars[i].cm);
    }
    char str[25];
    if((double)k_total != (double)1.0){
      unbalanced = true;
    }else{
      printf("\nBALANCED");
      unbalanced = false;
    }

    pthread_mutex_unlock(&write_mutex);

    if(changed_direction){
      sleep(CHANGE_DIRECTION);
    }
    sleep(MOVEMENT_TIME);

    sprintf(str,"id=%ld&cm=%d",b->id, b->cm);
    doPost("barValue",str);

    /* printf("\nEnding thread %ld", b->id); */
    changed_direction = false;
    pthread_mutex_unlock(&bar_mutex);
    sleep(1); 
  }
  pthread_exit(NULL);
}

void
fill_bar(struct bar* b, long id){
  b->id = id;
  b->cm = 0;
  b->direction = DOWN;
}

double
getDeltaKValue(int cm){
  switch (cm) {

  case 0: {
    return 0;
  }

  case 10: {
    return 0.1;
  }
  case 20: {
    return 0.4;
  }
  case 30: {
    return 0.6;
  }

  case -10: {
    return -0.1;
  }

  case -20: {
    return -0.4;
  }

  case -30: {
    return -0.6;
  }

default:
  return 0;
  }

}

void init_variables(){
  timeUnstabilized = 0.0;
  unbalanced = false;
  unstable_value = 0.0;
  k_value = 1.0;
}

void
print_bars(struct bar* bars){
  for (int i = 0; i < NUM_THREADS; ++i) {
    printf("\nBARRA: ");
    printf("\nID: %ld", bars[i].id);
    printf("\nCM: %d", bars[i].cm);
  }
}
