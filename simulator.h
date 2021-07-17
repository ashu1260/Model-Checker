#define _GNU_SOURCE /* This must be done for all or none of include files */
#include <dlfcn.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
char *schedule;
int count_thr_cre = 0,first_time=0,check=0;
int flag = 0,entry=-1;
char *argc[] = {0};
char str[4]; char ltr1[2],ltr2[2];
int env[3],init[2];
pid_t child_pid, wpid;
int status = 0,top=-1,count=-1;
int schedule_array[3];
struct mc_yield{
  sem_t s;
} t[2];
/************************************************************************/
static void set_schedule_array(char *env_value1){
  int str_len = strlen(env_value1);
  for(int i=0;i<str_len;i++){
    top++;
    schedule_array[top] = env_value1[i] - 48;
  }
}
/**************************************************************************/
static void mc_sched_yield(int thread_name){  
// printf("\n !!! in mc_sched !!! \n"); 
  count++;
// printf("\n------ count = %d -----\n",count);
  if(count>=3) return;
  if(count==0 && first_time==0){
    sem_init(&t[0].s, 0, 0);
    sem_init(&t[1].s, 0, 0);
    first_time=1;
  }    
  if(thread_name==schedule_array[count]){
    if(count==2) { 
      if(thread_name==1) sem_post(&t[1].s);
      else sem_post(&t[0].s);
      flag=0;
    }
  return;
  }
  else{
    count--;
    if(count<=0 && thread_name==schedule_array[count+2] ) {
      check++;
      if(check>1){
        if(thread_name==1) sem_post(&t[1].s);
        if(thread_name==2) sem_post(&t[0].s);
      }
      sem_wait(&t[thread_name-1].s); 
      return;
    }
    if(count==1){
      if(thread_name==1) sem_post(&t[1].s);
      if(thread_name==2) sem_post(&t[0].s);
      sem_wait(&t[thread_name-1].s);
      return;
    }
    else{
      sem_post(&t[schedule_array[count+2]-1].s);
      sem_wait(&t[thread_name-1].s);
      return;
    }   
  }
}

/********************** CONSTRUCTOR *****************************/

void __attribute__((constructor))
set_schedule() {
  for(int i=1;i<=2;i++){
    env[2]=i;
    for(int j=1;j<=2;j++){
      env[1]=j;
      for(int k=1;k<=2;k++){
        env[0]=k;
        sprintf(str,"%d%d%d",env[0],env[1],env[2]);
        if((child_pid =fork()) == 0){
          setenv("SCHEDULE",str,0);
          schedule = getenv("SCHEDULE");
          printf("\n****** schedule  %s is set in constructor ******\n",str);
          set_schedule_array(schedule);
          return;
        }
        while ((wpid = wait(&status)) > 0);
        printf("--------------------\n");
      }
    }
            
  }
  exit(0); 
}
/**************************************************************/
// function pointer  (preferred for readability)
typedef int (*pthread_create_t)(pthread_t *thread, const pthread_attr_t *attr,
  void *(*start_routine) (void *), void *arg);
pthread_create_t original_pthread_create = NULL;

typedef int (*sem_wait_t)(sem_t *sem);
sem_wait_t original_sem_wait = NULL;

typedef int (*pthread_mutex_init_t)(pthread_mutex_t *mutex, 
  const pthread_mutexattr_t *attr);
pthread_mutex_init_t original_pthread_mutex_init = NULL;

typedef int (*pthread_mutex_lock_t)(pthread_mutex_t *mutex);
pthread_mutex_lock_t original_pthread_mutex_lock = NULL;


/*************** Wrapper for pthread_create() ***************/

void load_original_pthread_create() {
  void *handle = dlopen("/lib/x86_64-linux-gnu/libpthread.so.0", RTLD_LAZY);
  char *err = dlerror();
  if (err) printf("%s\n", err);

  // Use absolute handle or RTLD_NEXT (next library defining pthread_create)
  // original_pthread_create = dlsym(handle, "pthread_create");
  original_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
  err = dlerror();
  if (err) printf("%s\n", err);
}
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
  void *(*start_routine) (void *), void *arg) {

  count_thr_cre++;
  // condition so that only one thread is created
  // printf("\n!!! flag = %d !!!\n",flag);
  while(flag==1);     
  flag=1;             
  if (original_pthread_create == NULL) load_original_pthread_create();
// printf("\n ------ pthread wrapper -----\n");
  return original_pthread_create(thread, attr, start_routine, arg);

}
/*************** Wrapper for sem_wait() ***************/

void load_original_sem_wait() {
  void *handle1 = dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
  char *err1 = dlerror();
  if (err1) printf("%s\n", err1);
    
  // Use absolute handle or RTLD_NEXT (next library defining sem_wait)
    
  original_sem_wait = dlsym(RTLD_NEXT, "sem_wait");
  err1 = dlerror();
  if (err1) printf("%s\n", err1);

}

int sem_wait(sem_t *sem){
  if(count_thr_cre >=2 && flag==1) flag = 0;
  if(original_sem_wait == NULL) load_original_sem_wait();
  return original_sem_wait(sem);
}

/*************** Wrapper for pthread_mutex_init() ***************/

void load_original_pthread_mutex_init() {
  void *handle2 = dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
  char *err2 = dlerror();
  if (err2) printf("%s\n", err2);
    
  // Use absolute handle or RTLD_NEXT (next library defining sem_wait)
    
  original_pthread_mutex_init = dlsym(RTLD_NEXT, "pthread_mutex_init");
  err2 = dlerror();
  if (err2) printf("%s\n", err2);

}

int pthread_mutex_init(pthread_mutex_t *mutex, 
    const pthread_mutexattr_t *attr){
   
    entry++;
    if(entry == 0){
      sprintf(ltr1,"%p",mutex);
      setenv("LOCK1",ltr1,0);
    }
    if(entry == 1){
      sprintf(ltr2,"%p",mutex);
      setenv("LOCK2",ltr2,0);
    }
    if(original_pthread_mutex_init== NULL) load_original_pthread_mutex_init();
  //  printf("\n *** mutex initiated ****\n");
    return original_pthread_mutex_init(mutex,attr);
}

/*************** Wrapper for pthread_mutex_lock() ***************/
void load_original_pthread_mutex_lock() {
  void *handle3 = dlopen("/lib/x86_64-linux-gnu/libc.so.6", RTLD_LAZY);
  char *err3 = dlerror();
  if (err3) printf("%s\n", err3);
    
  // Use absolute handle or RTLD_NEXT (next library defining sem_wait)
    
  original_pthread_mutex_lock = dlsym(RTLD_NEXT, "pthread_mutex_lock");
  err3 = dlerror();
  if (err3) printf("%s\n", err3);

}
int pthread_mutex_lock(pthread_mutex_t *mutex){
    char m[200];
    sprintf(m,"%p",mutex);
    char *e_v1 = getenv("LOCK1");
    char *e_v2 = getenv("LOCK2");
    if(e_v1 != NULL && !strcmp(e_v1,m)) mc_sched_yield(1);
    if(e_v2 != NULL && !strcmp(e_v2,m)) mc_sched_yield(2);
    if(original_pthread_mutex_lock== NULL) load_original_pthread_mutex_lock();
    // printf("\n^^^^ wrapper of lock ^^^\n");
    return original_pthread_mutex_lock(mutex);
}


