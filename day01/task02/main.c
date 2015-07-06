#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int num = 0;

bool p = true;
bool end = false;

void ping(void *a)
{
	do
	{
		pthread_mutex_lock(&mutex);
		if (p)
		{
			printf("Ping\n");
			p=false;
		}
		pthread_mutex_unlock(&mutex);
		//pthread_yield();
	}
	while (!end);

}

void pong(void *a)
{
	do
	{
		pthread_mutex_lock(&mutex);
		if (!p)
		{
			printf("\tPong\n");
			p=true;
			--num;
			if (!num)
				end = true;
		}
		pthread_mutex_unlock(&mutex);
		//pthread_yield();
	}
	while (!end);
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("podaj ilosc w argumencie\n");
		return 1;
	}

	num = atoi(argv[1]);

	pthread_t tid[2];

	pthread_create(&tid[0], NULL, (void*)ping, &cond);
	pthread_create(&tid[1], NULL, (void*)pong, &cond);

	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);



	return 0;
}




