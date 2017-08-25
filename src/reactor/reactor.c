#include "reactor.h"

//TODO: crear nuevo hilo para notificar el tiempo que lleva desestabilizado el reactor
void
start_threads(struct bar* bars){
  int i;
  pthread_t threads[NUM_THREADS + 3];
  init_variables();
  init_turn_conds();
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_mutex_init(&readK_mutex, NULL);
  int res = sem_init(&write_mutex, 0, 0);
  if (res < 0)
    {
      perror("Semaphore initialization failed");
      exit(0);
    }
  if (sem_init(&write_mutex, 0, 1)) /* initially unlocked */
    {
      perror("Semaphore initialization failed");
      exit(0);
    }

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
  pthread_create(&threads[NUM_THREADS + 2], &attr, count_unstable, (void *)bars);
  pthread_create(&threads[NUM_THREADS + 1], &attr, ask_value, (void *)bars);

  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS + 2; i++) {
    pthread_join(threads[i], NULL);
  }

}

void*
ask_value(void* bars){
  while(true){
    if(system_off == true){
      pthread_exit(NULL);
    }
    printf("\nREADING VALUE FROM USER: ");
    unstable_value = read_unstable_value();
    printf("\nValor leido: %lf\n", unstable_value);
  }
}

void*
count_unstable(void* bars){
  clock_t start = clock();
  while(true){
    sem_wait(&write_mutex);
    printf("GETTING THE WRITE MUTEX.....");
    if(1.5 > (double)k_total > 0.5){
      start = clock();
    }
    printf("\nVALOR TOTAL: %lf, %lf, %lf", ((double) (clock() - start)) / CLOCKS_PER_SEC, (double)k_total, k_total);
    printf("release THE WRITE MUTEX.....");
    sem_post(&write_mutex);
    sleep(1);

    if(((double) (clock() - start)) / CLOCKS_PER_SEC > MAX_BALANCE_TIME){
      printf("\nTerminacioooooooooooooooon...");
      pthread_mutex_lock(&bar_mutex);
      system_off = true;
      pthread_mutex_unlock(&bar_mutex);
      sleep(1);
    }
  }
}

void*
check_stable(void* bars){
  struct bar* wires = (struct bar*) bars ;
  while (true) {
    if(system_off == true){
      pthread_exit(NULL);
    }
    pthread_mutex_lock(&bar_mutex);
    sem_wait(&write_mutex);
    printf("\nCHECKSTABLE getting THE WRITE MUTEX.....");
    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += getKValue(wires[i].cm);
    }

    char str[25];
    sprintf(str,"deltak=%lf", k_total);
    doPost("deltak",str);

    pthread_mutex_lock(&readK_mutex);
    k_total = k_value + k_total;
    pthread_mutex_unlock(&readK_mutex);

    sprintf(str,"kparcial=%lf",k_value);
    doPost("kparcial",str);
    sprintf(str,"ktotal=%lf", k_total);
    doPost("ktotal",str);
    if((double)k_total != (double)1.0){
      if(unbalanced == false)
        pthread_cond_broadcast(&unstable_state);
      unbalanced = true;
    }else{
      unbalanced = false;
    }
    printf("\nCHECKSTABLE release THE WRITE MUTEX.....k_total %lf", k_total);
    sem_post(&write_mutex);
    usleep(500);
    pthread_mutex_unlock(&bar_mutex);
    usleep(500);
  }
}

double
read_unstable_value(){
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &unstable_value);

  pthread_mutex_lock(&readK_mutex);
  k_value = k_value + unstable_value; // de una vez calculamos el valor de k
  pthread_mutex_unlock(&readK_mutex);

  return k_value;
}

void*
move_bar(void *bar){
  struct bar* b = (struct bar*) bar ;
  bool changed_direction = false;
  while(true){
    // if(system_off == true){
    //   pthread_exit(NULL);
    // }
    pthread_mutex_lock(&mutex_cond[b->id - 1]);
    while(turn[b->id-1] == false){
      pthread_cond_wait(&turn_cond[b->id - 1], &mutex_cond[b->id - 1]);
    }
    sem_wait(&write_mutex);
    printf("MOVE BAR GETTING THE WRITE MUTEX.....");
    if (k_total < 1 && b->cm <=20) {
      b->cm = b->cm + 10; //Usando este valor calculamos el deltak
      /* printf("\nThread yendo hacia abajo\n"); */
      if(b->direction == UP){
        b->direction = DOWN;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }else if(k_total > 1 && b->cm >=10){
      b->cm = b->cm - 10;
      if(b->direction == DOWN){
        b->direction = UP;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }

    sem_post(&write_mutex);

    printf("\nMOVE BAR release THE WRITE MUTEX.....with k_total %lf", k_total);
    // pthread_mutex_unlock(&bar_mutex);
    pthread_mutex_unlock(&mutex_cond[b->id - 1]);

    if(changed_direction){
      sleep(CHANGE_DIRECTION);
    }
    sleep(MOVEMENT_TIME);
    sem_wait(&write_mutex);
    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += bars[i].deltak;
      printf("\nvalor: %lf", getKValue(bars[i].cm));
    }
    k_total = k_value + k_total;
    char str[25];
    if((double)k_total != (double)1.0){
      unbalanced = true;
    }else{
      printf("\nBALANCED");
      unbalanced = false;
    }
    sprintf(str,"id=%ld&cm=%d",b->id, b->cm);
    doPost("barValue",str);
    sem_post(&write_mutex);

    /* printf("\nEnding thread %ld", b->id); */
    changed_direction = false;
    usleep(500);
  }
  pthread_exit(NULL);
}

void
fill_bar(struct bar* b, long id){
  b->id = id;
  b->cm = 0;
  b->deltak = 0;
  b->direction = DOWN;
}

double
getKValue(int cm){
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

  default:
    return 0;
  }

}

void init_variables(){
  system_off = false;
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

void
init_turn_conds(){
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_cond_init(&turn_cond[i], NULL);
    turn[i] = false;
    pthread_mutex_init(&mutex_cond[i], NULL);
  }
}

bool
areTurnsOff(){
  for (int i = 0; i < NUM_THREADS; ++i) {
    if(turn[i] == true)
      return false;
  }
  return true;
}
