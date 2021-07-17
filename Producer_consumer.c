#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
int flag_e = 0;

/*
This program provides a possible solution for producer-consumer problem using mutex and semaphore.
I have used 5 producers and 5 consumers to demonstrate the solution. You can always play with these values.
*/

#define MaxItems 3 // Maximum items a producer can produce or a consumer can consume
#define BufferSize 3 // Size of the buffer

// sem_t empty;
// sem_t full;
int in = 0;
int out = 0;
int buffer[BufferSize];
pthread_mutex_t mutexP;
pthread_mutex_t mutexC;


void *producer()
{
    // printf("\n **** in producer ***\n");
    int item;
    for(int i = 0; i < MaxItems; i++) {
        item = rand()%100; // Produce an random item
        if(item == 0) item++;
        pthread_mutex_lock(&mutexP);
        buffer[in] = item;
        pthread_mutex_unlock(&mutexP);
        printf("Producer Insert Item %d at %d\n", buffer[in],in); 
        in = (in+1)%BufferSize;
        
       
    }
}
void *consumer()
{
    // printf("\n **** in consumer ****\n");  
  
    for(int i = 0; i < MaxItems; i++) {
       
        pthread_mutex_lock(&mutexC);
        int item = buffer[out];
        pthread_mutex_unlock(&mutexC);
        if(item == 0){ flag_e = 1; exit(0);}
        printf("Consumer  Remove Item %d from %d\n",item, out);
        out = (out+1)%BufferSize;
       
       
    }
}

int main(int argc, char *argv[])
{ 
    // printf("\n **** in target main ***\n"); 
    pthread_mutex_init(&mutexP, NULL);
    pthread_mutex_init(&mutexC, NULL);
    // pthread_t pro[5],con[5];
     pthread_t pro,con;
    

    int a[5] = {1,2,3,4,5}; //Just used for numbering the producer and consumer

    // for(int i = 0; i < 5; i++) {
    //     pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    // }
    // for(int i = 0; i < 5; i++) {
    //     pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    // }

    // for(int i = 0; i < 5; i++) {
    //     pthread_join(pro[i], NULL);
    // }
    // for(int i = 0; i < 5; i++) {
    //     pthread_join(con[i], NULL);
    // }
    pthread_create(&pro, NULL, (void *)producer, NULL);
    pthread_create(&con, NULL, (void *)consumer, NULL);
    pthread_join(pro,NULL);
    pthread_join(con,NULL);
    // printf("\n ----- back in target main -----\n");
    if(flag_e==1) printf("!!! BAD SCHEDULE !!!\n");
    pthread_mutex_destroy(&mutexP);
    pthread_mutex_destroy(&mutexC);
    
    // sem_destroy(&empty);
    // sem_destroy(&full);

    return 0;
    
}