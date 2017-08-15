#include "reactor.h"

//TODO: crear nuevo hilo para notificar el tiempo que lleva desestabilizado el reactor
void
start_threads(struct bar* bars){
  int i;
  pthread_t threads[NUM_THREADS];
  unstable_value = 0.0;
  k_value = 0.0;
  pthread_mutex_init(&bar_mutex, NULL);
  pthread_cond_init(&unstable_state, NULL);

  for (i = 1; i < 5; i++) {
    fill_bar(&bars[i - 1], i, &k_value);
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&threads[0], &attr, move_bar, (void *)&bars[0]);
  pthread_create(&threads[1], &attr, move_bar, (void *)&bars[1]);
  pthread_create(&threads[2], &attr, move_bar, (void *)&bars[2]);
  pthread_create(&threads[3], &attr, move_bar, (void *)&bars[3]);


  printf("\nK - Value is: %lf", k_value);
  printf("\nREADING VALUE FROM USER: ");
  while(true){
    unstable_value = read_unstable_value();
  }
  
  /* Wait for all threads to complete */
  for (i=0; i<NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }
}

/* TODO: Validar rango de valor a desastibilizar, 
   tipo de variable ingresada... 
   Manejo de excepciones */
double
read_unstable_value(){
  double val = 0;
  printf("\nEnter the value to unstabilize the system: ");
  scanf("%lf", &val);
  return val;
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
fill_bar(struct bar* b, long id, double* k_value){
  b->id = id;
  b->k_value = k_value;
  b->direction = DOWN;
}

// TODO: retornar el delta k dependiendo de la distancia dada
double
getdeltakvalue(int cm){
  return 1.0;
}
