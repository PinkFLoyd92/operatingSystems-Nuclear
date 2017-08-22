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


void
check_stable(void* bars){
  struct bar* wires = (struct bar*) bars ;
  for (int i = 0; i < NUM_THREADS; ++i) {
    printf("Wire Value: %ld", wires[i].id);
  }

}

/* TODO: Validar rango de valor a desastibilizar, 
   tipo de variable ingresada... 
   Manejo de excepciones */
double
read_unstable_value(){
  pthread_mutex_lock(&write_mutex);
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &unstable_value);
  k_value = k_value + unstable_value; // de una vez calculamos el valor de k
  pthread_mutex_unlock(&write_mutex);
}

// TODO: MOVER LA BARRA... CONSIDERAR TIEMPOS PARA LA TERMINACION DE LA RUTINA DEL THREAD
void*
move_bar(void *bar){
  struct bar* b = (struct bar*) bar ;
  double delta_k = 0.0;
  int future_cm = 0;
  clock_t start_t;
  bool changed_direction = false;
  printf("Starting bar thread: id-> %ld", b->id);
  while(true){
    pthread_mutex_lock(&bar_mutex);
    //cuando se encuentre desbalanceado podremos ingresar y editar.
    while(!unbalanced)
      pthread_cond_wait(&unstable_state, &bar_mutex);

    start_t = clock(); //we begin to run the clock
    //printf("Moviendo barra usando el hilo: %ld\n", my_id);
    if (*(b->k_value) > 1) {
      future_cm = 10 + b->cm; //Usando este valor calculamos el deltak
      delta_k = getDeltaKValue(future_cm);
      if(b->direction == DOWN){
        b->direction = UP;
        changed_direction = true;
      }else{
        changed_direction = false;
      }
      *(b->k_value) -= delta_k;
       
    }else if((*(b->k_value) < 1) ){
      future_cm = b->cm - 10; //Usando este valor calculamos el deltak
      delta_k = getDeltaKValue(future_cm);
      *(b->k_value) -= delta_k;
      if(b->direction == UP){
        b->direction = DOWN;
        changed_direction = true;
      }else{
        changed_direction = false;
      }

    }
    //modificamos el valor de la barra
    //revisamos si debemos hacer el movimiento hacia arriba o abajo
    pthread_mutex_unlock(&bar_mutex);
    if(changed_direction){
      printf("We have to change the direction");
      while((clock() - start_t)/CLOCKS_PER_SEC < CHANGE_DIRECTION); // changing direction
    }
    start_t = clock();
    while((clock() - start_t)/CLOCKS_PER_SEC < MOVEMENT_TIME); // bar's movement
    changed_direction = false;
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
getdeltakvalue(int cm){
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
  k_value = 0.0;
  k_total = 0.0;
}
