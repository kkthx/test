#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void *f1(void *a)
{
	pthread_mutex_lock(&mutex);

	pthread_cond_wait(&cond, &mutex);

	printf("Woked up!\n");

	pthread_mutex_unlock(&mutex);

	return 0;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("podaj ilosc w argumencie\n");
		return 1;
	}

	pthread_t tid;

	pthread_create(&tid, NULL, (void*)f1, &cond);

	sleep(1);
	pthread_mutex_lock(&mutex);
	printf("Wake up!\n");
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_join(tid, NULL);



	return 0;
}




