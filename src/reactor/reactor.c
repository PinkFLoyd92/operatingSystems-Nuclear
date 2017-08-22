#include "reactor.h"

//TODO: crear nuevo hilo para notificar el tiempo que lleva desestabilizado el reactor
void
start_threads(struct bar* bars){
  int i;
  pthread_t threads[NUM_THREADS + 1];
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

  while(true){
    printf("\nREADING VALUE FROM USER: ");
    unstable_value = read_unstable_value();
    printf("\nValor leido: %lf\n", unstable_value);
  }
  
  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS + 1; i++) {
    pthread_join(threads[i], NULL);
  }
}


void*
check_stable(void* bars){
  struct bar* wires = (struct bar*) bars ;
  while (true) {
    k_total = 0.0;
    pthread_mutex_trylock(&bar_mutex);
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += getDeltaKValue(wires[i].cm);
    }

    k_total = k_value + k_total;
    printf("\nValor del k_total: %lf\n", k_total);
    if(k_total != 1.0){
      unbalanced = true;
      printf("\nState k_total: %lf, ", k_total);
      print_bars(bars);
      pthread_cond_broadcast(&unstable_state);
    }else{
      printf("\nBALANCED");
      unbalanced = false;
    }
    pthread_mutex_unlock(&bar_mutex);
    sleep(1);
  }
}

/* TODO: Validar rango de valor a desastibilizar, 
   tipo de variable ingresada... 
   Manejo de excepciones */
double
read_unstable_value(){
  // pthread_mutex_lock(&write_mutex);
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &unstable_value);
  k_value = k_value + unstable_value; // de una vez calculamos el valor de k
  // pthread_mutex_unlock(&write_mutex);
  return k_value;
}

// TODO: MOVER LA BARRA... CONSIDERAR TIEMPOS PARA LA TERMINACION DE LA RUTINA DEL THREAD
void*
move_bar(void *bar){
  struct bar* b = (struct bar*) bar ;
  clock_t start_t;
  bool changed_direction = false;
  printf("Starting bar thread: id-> %ld", b->id);
  while(true){
    pthread_mutex_trylock(&bar_mutex);
    //cuando se encuentre desbalanceado podremos ingresar y editar.
    while(unbalanced == false)
      pthread_cond_wait(&unstable_state, &bar_mutex);

    start_t = clock(); //we begin to run the clock
    if (k_total < 1 && b->cm <=20) {
      b->cm = b->cm + 10; //Usando este valor calculamos el deltak
      printf("\nThread yendo hacia abajo\n");
      if(b->direction == UP){
        b->direction = DOWN;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }else if(k_total > 1 && b->cm >=-20){
      b->cm = b->cm - 10; //Usando este valor calculamos el deltak
      printf("\nThread yendo hacia arriba\n");
      if(b->direction == DOWN){
        b->direction = UP;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
    }
    // calculo de k_total
    k_total = 0.0;
    for (int i = 0; i < NUM_THREADS; ++i) {
      k_total += getDeltaKValue(bars[i].cm);
    }

    k_total = k_value + k_total;
    doPost("LLALALALA");
    printf("\nValor del k_total, %lf k_value: %lf\n", k_total);
    print_bars(bars);
    if(k_total != 1.0){
      unbalanced = true;
      printf("\nUNBALANCED");
      pthread_cond_broadcast(&unstable_state);
    }else{
      printf("\nBALANCED");
      unbalanced = false;
    }
    pthread_mutex_unlock(&bar_mutex);
    sleep(1);
    // if(changed_direction){
    //   printf("We have to change the direction");
    //   while((clock() - start_t)/CLOCKS_PER_SEC < CHANGE_DIRECTION); // changing direction
    // }
    // start_t = clock();
    // while((clock() - start_t)/CLOCKS_PER_SEC < MOVEMENT_TIME); // bar's movement
    // changed_direction = false;
  }
  pthread_exit(NULL);
}

/*
TODO: verificar que se llenen exitosamente estos datos
*/ 
void
fill_bar(struct bar* b, long id){
  b->id = id;
  b->cm = 0;
  b->direction = DOWN;
}

// TODO: retornar el delta k dependiendo de la distancia dada
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
