#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

void f1(void)
{
	printf("nowy wątek, przed exeec \n   pid=%d\nparent=%d\n\n", getpid(), getppid());

	execl("/bin/ls", "ls", "-la", "--color", NULL);

}

int main(int argc, char **argv)
{
	pthread_t tid;

	printf("proces przed pthread_create\n   pid=%d\nparent=%d\n\n", getpid(), getppid());

	pthread_create(&tid, NULL, (void*)f1, NULL);

	printf("proces po pthread_create\n   pid=%d\nparent=%d\n\n", getpid(), getppid());

	pthread_join(tid, NULL);






	return 0;
}




