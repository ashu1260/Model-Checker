#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>

char *env_value , *sched_ptr;
unsigned long int pt;
static volatile int var_A = 0;
static volatile int var_B = 0;
int flag_thread =0;

pthread_mutex_t lock1;
pthread_mutex_t lock2;

void *mythread1() {
  pthread_mutex_lock(&lock1); 
  if (var_B == 0) {
    pthread_mutex_unlock(&lock1);        
    pthread_mutex_lock(&lock1);
    var_A = 1;
  }
  pthread_mutex_unlock(&lock1);
  pthread_mutex_lock(&lock1);
  if (var_A) {printf("I'm the leader(A).\n"); fflush(stdout);}
  pthread_mutex_unlock(&lock1);
  
}

void *mythread2() {
  pthread_mutex_lock(&lock2); 
  if (var_A == 0) {
    pthread_mutex_unlock(&lock2);        
    pthread_mutex_lock(&lock2);
    var_B = 1;
    
  }
  pthread_mutex_unlock(&lock2);
  pthread_mutex_lock(&lock2);
  if (var_B) {printf("I'm the leader(B).\n"); fflush(stdout);}
  pthread_mutex_unlock(&lock2);
}

int main(int argc, char *argv[]){
  env_value = getenv("SCHEDULE");
  pthread_mutex_init(&lock1, NULL);
  pthread_mutex_init(&lock2, NULL);
  pthread_t P_A,P_B;   
  int rc; 
  rc = pthread_create(&P_A, NULL, mythread1, NULL);
  assert(rc == 0);
  rc = pthread_create(&P_B, NULL, mythread2, NULL);
  assert(rc == 0);
  pthread_join(P_A, NULL);
  pthread_join(P_B, NULL);
  if(var_A==1 && var_B == 1) printf("*-*-*- BAD SCHEDULE IS %s *-*-*-\n",env_value); 
  fflush(stdout);
  printf("main: done with both (A = %d, B = %d)\n", var_A, var_B);
  fflush(stdout);
  pthread_mutex_destroy(&lock1);
  pthread_mutex_destroy(&lock2);
  return 0;
}