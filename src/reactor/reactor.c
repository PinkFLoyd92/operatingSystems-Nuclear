#include "reactor.h"

void
start_threads(struct bar* bars){
  int i;
  pthread_t threads[NUM_THREADS + 3];
  init_variables();
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_mutex_init(&read_unstable_mutex, NULL);
  int res = sem_init(&write_mutex, 0, 0);
if (res < 0) {
        perror("Semaphore initialization failed");
        exit(0);
    }
 if (sem_init(&write_mutex, 0, 1)) {
   perror("Semaphore initialization failed");
   exit(0);
 }

  pthread_cond_init(&unstable_state, NULL);

  for (i = 1; i < NUM_THREADS + 1; i++) {
    fill_bar(&bars[i - 1], i);
  }
  print_bars(bars);
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

// This is runnning in a thread checking the time that the system is unstable...
void*
count_unstable(void* bars){
  clock_t start = clock();
  while(true){
    sem_wait(&write_mutex);
    if(1.5 > (double)k_total > 0.5){
      start = clock(); // here we restart the clock
    }
    printf("\nVALOR TOTAL: %lf, %lf, %lf", ((double) (clock() - start)) / CLOCKS_PER_SEC, (double)k_total, k_total);
    // printf("release THE WRITE MUTEX.....");
    sem_post(&write_mutex);
    usleep(500);

    if(((double) (clock() - start)) / CLOCKS_PER_SEC > MAX_BALANCE_TIME){
      pthread_mutex_lock(&bar_mutex);
      printf("lalalalalaaaaaaaaaaaa");
      system_off = true;
      pthread_mutex_unlock(&bar_mutex);
      usleep(500);
    }
  }
}

void*
check_stable(void* bars){
  struct bar* wires = (struct bar*) bars ;
  bool turn_available = false;
  int selected_bar = -1;
  srand ( time(NULL) );
  while (true) {
    if(system_off == true){
      pthread_exit(NULL);
    }
    pthread_mutex_lock(&bar_mutex);
    sem_wait(&write_mutex);
    // printf("\nCHECKSTABLE getting THE WRITE MUTEX.....");
    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += wires[i].deltak;
    }
    char str[25];
    sprintf(str,"deltak=%lf", k_total);
    doPost("deltak",str);

    pthread_mutex_lock(&read_unstable_mutex);
    k_total = k_value + k_total;
    sprintf(str,"kparcial=%lf",k_value);
    doPost("kparcial",str);
    sprintf(str,"ktotal=%lf", k_total);
    doPost("ktotal",str);

    pthread_mutex_unlock(&read_unstable_mutex);

    if((double)k_total != (double)1.0){
      if(unbalanced == false)
        pthread_cond_broadcast(&unstable_state);
      unbalanced = true;
      turn_available = is_turn_available(); // we check if we have an available turn
      if(turn_available){
        selected_bar = (rand() % NUM_THREADS) + 1; //  selecting bar 1 to 16
        if( turn[selected_bar - 1] == true){
          selected_bar = select_first_bar_available();
        }
        turn[selected_bar - 1] =  true;
        pthread_cond_signal(&cond_bar[selected_bar - 1]);
        turn[selected_bar - 1] =  true;

        pthread_cond_signal(&cond_bar[get_opposite_bar(selected_bar) - 1]);
        turn[get_opposite_bar(selected_bar) - 1] =  true;
      }
      printf("\nDesbalanceado......");
    }else{
      unbalanced = false;
      printf("\nBalanceado......");
    }
    sem_post(&write_mutex);
    pthread_mutex_unlock(&bar_mutex);
    sleep(1);
  }
}

double
read_unstable_value(){
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &unstable_value);
  pthread_mutex_lock(&read_unstable_mutex);
  k_value = k_value + unstable_value; // de una vez calculamos el valor de k
  pthread_mutex_unlock(&read_unstable_mutex);
  return k_value;
}

// solo alteramos el valor de b->deltak
void*
move_bar(void *bar){
  struct bar* b = (struct bar*) bar ;
  bool changed_direction = false;
  while(true){
    pthread_mutex_lock(&bar_mutex);
    if(system_off == true){
      pthread_exit(NULL);
    }
    while(unbalanced == false){
      pthread_cond_wait(&unstable_state, &bar_mutex);
    }
    // here we wait for the check_stable thread to activate this one.
    while(turn[b->id - 1] == false){
      pthread_cond_wait(&cond_bar[b->id - 1], &bar_mutex);
    }

    sem_wait(&write_mutex);
    if (k_total < 1 && b->cm <=20) {
      if(b->direction == UP){
        b->direction = DOWN;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
      printf("\nMoviendo hacia abajo: %ld", b->id);
    }else if(k_total > 1 && b->cm >=10){
      if(b->direction == DOWN){
        b->direction = UP;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
      printf("\nMoviendo hacia arriba barra: %ld", b->id);
    }else{
      printf("La barra esta estatica");
      //b->deltak = 0;
    }
    sem_post(&write_mutex);
    usleep(500);
    pthread_mutex_unlock(&bar_mutex);
    usleep(500);
    // aqui ejecutamos el movimiento
    if(changed_direction){
      sleep(CHANGE_DIRECTION); //animacion del cambio de direccion
    }
    sleep(MOVEMENT_TIME); // animacion del cambio de movimiento
    sem_wait(&write_mutex);
    printf("\nMOVEBAR getting THE WRITE MUTEX.....ID: %ld", b->id);
    if (k_total < 1 && b->cm <=20) {
      b->cm = b->cm + 10; //Usando este valor calculamos el deltak
      b->deltak = getDeltaKValue(b->cm) - getDeltaKValue(b->cm -10) ;
    }else if(k_total > 1 && b->cm >=10){
      b->cm = b->cm - 10;
      b->deltak = getDeltaKValue(b->cm) - getDeltaKValue(b->cm +10) ;
    }

    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += bars[i].deltak;
    }

    char str[25];
    sprintf(str,"deltak=%lf", k_total);
    doPost("deltak",str);

    pthread_mutex_lock(&read_unstable_mutex);
    sprintf(str,"id=%ld&cm=%d",b->id, b->cm);
    doPost("barValue",str);
    k_total = k_value + k_total;
    sprintf(str,"kparcial=%lf",k_value);
    doPost("kparcial",str);
    sprintf(str,"ktotal=%lf", k_total);
    doPost("ktotal",str);

    pthread_mutex_unlock(&read_unstable_mutex);

    if((double)k_total != (double)1.0){
      if(unbalanced == false)
        pthread_cond_broadcast(&unstable_state);
      unbalanced = true;
    }else{
      unbalanced = false;
    }
    printf("\nMOVEBAR releasing THE WRITE MUTEX.....ID: %ld", b->id);
    turn[b->id - 1] = false;
    sem_post(&write_mutex);
    usleep(500);

  }
  pthread_exit(NULL);
}

void
fill_bar(struct bar* b, long id){
  b->id = id;
  b->cm = 0;
  b->direction = DOWN;
  b->deltak = 0.0;
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

  default:
    return 0;
  }

}

void init_variables(){
  system_off = false;
  unbalanced = false;
  unstable_value = 0.0;
  k_value = 1.0;
  for (int i = 0; i < NUM_THREADS; ++i) {
    turn[i] = false;
    pthread_cond_init(&cond_bar[i], NULL);
  }

}

void
print_bars(struct bar* bars){
  for (int i = 0; i < NUM_THREADS; ++i) {
    printf("\nBARRA: ");
    printf("\nID: %ld", bars[i].id);
    printf("\nCM: %d", bars[i].cm);
  }
}

bool
is_turn_available(){
  for (int i = 0; i < NUM_THREADS; ++i) {
    if(turn[i] == true)
      return false;
  }
  return true;

}

// send a bar id: 1 to 16 and returns its opposite e.g 8 return 9 16 return 1
int
get_opposite_bar(int num){
    return NUM_THREADS - num + 1;
}


int
select_first_bar_available(){
  for (int i = 0; i < NUM_THREADS; ++i) {
    if(turn[i] == false){
      return i + 1;
    }
  }
  return -1;

}
