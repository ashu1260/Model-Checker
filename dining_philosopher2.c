#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_MEALS 7
#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 3
#define NOTHING 0
#define LEFT (phnum + 4) % N
#define RIGHT (phnum + 1) % N

int state[N]={0};
int phil[N] = { 0, 1, 2, 3, 4 };
int count_meal[N] = {0};

pthread_mutex_t mutex;
// sem_t S[N];
pthread_mutex_t S[N];

void exit_gracefully(){
    printf("\n **** STASTICS: ****\n");
    for(int i=0;i<N;i++){
        printf("Philosopher %d ate %d meals.\n",i+1,count_meal[i]);
    }
    printf("\n Ended successfully \n");
    exit(0);
}


void test(int phnum)
{	
	// printf("\n in test\n");
	// printf("STATE of philosopher is %d and left is %d and right is %d",state[phnum],state[LEFT],state[RIGHT]);
	if (state[phnum] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING) {
		// state that eating
		// printf("\n in if \n");
		state[phnum] = EATING;

		sleep(2);

		printf("Philosopher %d takes fork %d and %d\n",
					phnum + 1, LEFT + 1, phnum + 1);
		
		printf("Philosopher %d is Eating\n", phnum + 1);

		// sem_post(&S[phnum]) has no effect
		// during takefork
		// used to wake up hungry philosophers
		// during putfork
		// sem_post(&S[phnum]);
		pthread_mutex_unlock(&S[phnum]);
	}
}

// take up chopsticks
void take_fork(int phnum)
{
	// printf("\n in take fork\n");
	pthread_mutex_lock(&mutex);

	// state that hungry
	state[phnum] = HUNGRY;

	printf("Philosopher %d is Hungry\n", phnum + 1);

	// eat if neighbours are not eating
	test(phnum);

	pthread_mutex_unlock(&mutex);

	// if unable to eat wait to be signalled
	pthread_mutex_lock(&S[phnum]);

	sleep(1);
}

// put down chopsticks
void put_fork(int phnum)
{	
	// printf("\n in put fork\n");
    static int meal_no = 0;
	pthread_mutex_lock(&mutex);

	// state that thinking
	state[phnum] = THINKING;

	printf("Philosopher %d putting fork %d and %d down\n",
		phnum + 1, LEFT + 1, phnum + 1);
    count_meal[phnum]++;
    meal_no++;
	printf("Philosopher %d is thinking\n", phnum + 1);
    if (meal_no == MAX_MEALS){
        exit_gracefully();
    }
	pthread_mutex_unlock(&mutex);
	// sleep(6);
	pthread_mutex_lock(&mutex);
	test(LEFT);
	test(RIGHT);

	pthread_mutex_unlock(&mutex);
}

void* philospher(void* num)
{

	while (1) {

		int* i = num;

		sleep(1);

		take_fork(*i);

		sleep(0);

		put_fork(*i);
	}
}

int main()
{

	int i;
	pthread_t thread_id[N];
    
	// initialize the semaphores
	

	for (i = 0; i < N; i++)
		   pthread_mutex_init(&S[i],NULL);
		// sem_init(&S[i], 0, 0);
	pthread_mutex_init(&mutex,NULL);

	for (i = 0; i < N; i++) {

		// create philosopher processes
		pthread_create(&thread_id[i], NULL,
					philospher, &phil[i]);

		printf("Philosopher %d is thinking\n", i + 1);
	}

	for (i = 0; i < N; i++)

		pthread_join(thread_id[i], NULL);
}
