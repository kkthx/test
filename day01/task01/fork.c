#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void f1(void)
{
	printf("nowy wątek, przed fork \n   pid=%d\nparent=%d\n\n", getpid(), getppid());
	int f = fork();
	if (!f) // child
		printf("\nwątek po fork [dziecko]\n   pid=%d\nparent=%d\n\n", getpid(), getppid());
	else //parent
		printf("\nwątek po fork [rodzic]\n   pid=%d\nparent=%d\n\n", getpid(), getppid());

	//return 0;
}

int main(int argc, char **argv)
{
	pthread_t tid;

	printf("proces przed pthread_create\n   pid=%d\nparent=%d\n\n", getpid(), getppid());

	pthread_create(&tid, NULL, (void*)f1, NULL);

	pthread_join(tid, NULL);

	while(1)
		sleep(10);






	return 0;
}




