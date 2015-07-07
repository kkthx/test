#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFF_SIZE 1024

int srv_fd = -1;
int cli_fd = -1;
int epoll_fd = -1;
struct epoll_event e, es[100];

int i = 0;

ssize_t msg_len = -1;



struct sockaddr_in srv_addr;
struct sockaddr_in cli_addr;
socklen_t cli_addr_len;



void closeSockets(int arg)
{
	printf("Closing server\n");
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli_fd, &e);
	//close(cli_fd);
	//close(epoll_fd);
	//close(srv_fd);
}


int svHandler(void)
{

	cli_fd = accept(srv_fd, (struct sockaddr*) &cli_addr, &cli_addr_len);
	if (cli_fd < 0) {
		printf("Cannot accept client\n");
		close(epoll_fd);
		close(srv_fd);
		return 1;
	}

	e.data.fd = cli_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cli_fd, &e) < 0) {
		printf("Cannot accept client\n");
		close(epoll_fd);
		close(srv_fd);
		return 1;
	}
}




int main(int argc, const char *argv[])
{
	signal(SIGINT, closeSockets);
	signal(SIGTSTP, closeSockets);
	signal(SIGHUP, closeSockets);
	signal(SIGQUIT, closeSockets);

	char ip[] = "127.0.0.1";
	int port = 5580;

	if (argc == 2)
	{
		port = atoi(argv[1]);
	}





	memset(&srv_addr, 0, sizeof(srv_addr));
	memset(&cli_addr, 0, sizeof(cli_addr));
	memset(&e, 0, sizeof(e));

	srv_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (srv_fd < 0) {
		printf("Cannot create socket\n");
		return 1;
	}

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(port);
	if (bind(srv_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0) {
		printf("Cannot bind socket\n");
		close(srv_fd);
		return 1;
	}

	if (listen(srv_fd, 100) < 0) {
		printf("Cannot listen\n");
		close(srv_fd);
		return 1;
	}

	epoll_fd = epoll_create(100);
	if (epoll_fd < 0) {
		printf("Cannot create epoll\n");
		close(srv_fd);
		return 1;
	}

	e.events = EPOLLIN;
	e.data.fd = srv_fd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv_fd, &e) < 0) {
		printf("Cannot add server socket to epoll\n");
		close(epoll_fd);
		close(srv_fd);
		return 1;
	}

	for(;;) {
		i = epoll_wait(epoll_fd, es, 2, -1);
		if (i < 0) {
			printf("Cannot wait for events\n");
			close(epoll_fd);
			close(srv_fd);
			return 1;
		}

		for (--i; i > -1; --i) {
			if (es[i].data.fd == srv_fd) {
				svHandler();
			}
			//else {
			if (es[i].data.fd == cli_fd) {
				if (es[i].events & EPOLLIN)
				{
					char *buf;

					if (read(cli_fd, &msg_len, sizeof(size_t)) > 0)
					{
						buf = malloc(msg_len*sizeof(char));
						memset(buf, 0, msg_len);
						read(cli_fd, buf, msg_len);

						printf("msg len=%d\n", msg_len);

						printf("    msg=%s\n", buf);//+sizeof(ssize_t));

						fflush(stdout);

						write(cli_fd, &msg_len, sizeof(size_t));
						write(cli_fd, buf, msg_len);

						//if (strcmp(buff, "aa") == 0)
						//  printf("aa\n");
						free(buf);
					}

					else //
					{
						printf("Client disconnected");
						fflush(stdout);

						close(cli_fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli_fd, &e);
						//cli_fd = -1;
					}





					/*

					   if (read(cli_fd, &msg_len, sizeof(ssize_t)))
					   {
					   printf("msg len = %d\n", msg_len);
					   fflush(stdout);
					   memset(buff, 0, BUFF_SIZE);

					   read(cli_fd, buff, msg_len);
					   printf("msg = %s\n", buff);
					   fflush(stdout);
					   write(cli_fd, buff, msg_len);

					   if (strcmp(buff, "aa") == 0)
					   printf("aa\n");

					   }
					   else
					   {
					   printf("Client disconnected");
					   fflush(stdout);

					   close(cli_fd);
					   epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli_fd, &e);
					   }

*/


					/*

					//memset(buff, 0, BUFF_SIZE);

					if (msg_len > 0)
					{
					printf("Msg len: %d, msg: %s\n", msg_len, buff);
					fflush(stdout);
					write(cli_fd, buff, msg_len);

					if (strcmp(buff, "aa") == 0)
					printf("aa\n");

					}

					else //
					{
					printf("Client disconnected");
					fflush(stdout);

					close(cli_fd);
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli_fd, &e);
					//cli_fd = -1;
					}

					//epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cli_fd, &e);
					//cli_fd = -1;
					*/
				}
			}
		}
		}

		return 0;
	}

