#include <stdio.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>


#include "defs.h"
#include "users.h"


user users[MAXUSERS];


int srv_fd = -1;
int cli_fd = -1;
int epoll_fd = -1;
struct epoll_event e, es[MAXUSERS];

int i = 0;

ssize_t msg_len = -1;

struct sockaddr_in srv_addr;
struct sockaddr_in cli_addr;
socklen_t cli_addr_len;





void closeSockets(void)
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
    if (cli_fd < 0)
    {
        printf("Cannot accept client\n");
        close(epoll_fd);
        close(srv_fd);
        return 1;
    }

    e.data.fd = cli_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, e.data.fd, &e) < 0)
    {
        printf("Cannot accept client\n");
        close(epoll_fd);
        close(srv_fd);
        return 1;
    }

    return 0;
}




int clHandler(struct epoll_event *ev)
{

    printf("data.fd = %d\n", ev->data.fd);
    fflush(stdout);
    if (ev->events & EPOLLIN)
    {
        char *recv_buf = 0;

        if (read(ev->data.fd, &msg_len, sizeof(size_t)) > 0)
        {
            int result = -1;
            recv_buf = malloc(msg_len*sizeof(char)+1);
            memset(recv_buf, 0, msg_len+1);
            read(ev->data.fd, recv_buf, msg_len);


            switch (recv_buf[0])
            {
            case '2':
                printf("LogIn\n");
                login(users, ev->data.fd, recv_buf);
                break;
            case '3':
                printf("MessageTo\n");
                send_message(users, ev->data.fd, recv_buf);
                break;
            case '5':
                printf("Broadcast\n");
                send_message(users, ev->data.fd, recv_buf);
                break;
            case '6':
                printf("UserList\n");
                user_list(users, ev->data.fd);
                print_users(users);
                break;
            default:
				; //hack: http://stackoverflow.com/a/1181106
				// http://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
                char *send_buf = 0;
                send_buf = malloc(BUF_SIZE*sizeof(char)+1);
                strcpy(send_buf, "1.1.Unknown message: ");
                strcat(send_buf, recv_buf);
                send_data(ev->data.fd, send_buf);
                free(send_buf);
                break;

            }

            fflush(stdout);

        }
        else
        {
            printf("Client disconnected %d\n", ev->data.fd);
            delete_user_id(users, ev->data.fd);

            print_users(users);
            fflush(stdout);

            close(ev->data.fd);
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ev->data.fd, &e);
        }

        free(recv_buf);
    }

}


int main(int argc, const char *argv[])
{
    signal(SIGINT, closeSockets);
    signal(SIGTSTP, closeSockets);
    signal(SIGHUP, closeSockets);
    signal(SIGQUIT, closeSockets);

    char ip[] = "127.0.0.1";
    int port = PORT;



    if (argc == 2)
    {
        port = atoi(argv[1]);
    }

//memset(&users, 0, sizeof(user*MAXUSERS));
    for (int i=0; i<MAXUSERS; ++i)
    {
        users[i].id = -1;
    }

    printf("port: %d\n", port);
    fflush(stdout);



    memset(&srv_addr, 0, sizeof(srv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));
    memset(&e, 0, sizeof(e));


    srv_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (srv_fd < 0)
    {
        printf("Cannot create socket\n");
        return 1;
    }

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(port);
    if (bind(srv_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr)) < 0)
    {
        printf("Cannot bind socket\n");
        close(srv_fd);
        return 1;
    }

    if (listen(srv_fd, MAXUSERS) < 0)
    {
        printf("Cannot listen\n");
        close(srv_fd);
        return 1;
    }

    epoll_fd = epoll_create(MAXUSERS);
    if (epoll_fd < 0)
    {
        printf("Cannot create epoll\n");
        close(srv_fd);
        return 1;
    }

    e.events = EPOLLIN;
    e.data.fd = srv_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, srv_fd, &e) < 0)
    {
        printf("Cannot add server socket to epoll\n");
        close(epoll_fd);
        close(srv_fd);
        return 1;
    }

    for(;;)
    {
        i = epoll_wait(epoll_fd, es, MAXUSERS, -1);
        if (i < 0)
        {
            printf("Cannot wait for events\n");
            close(epoll_fd);
            close(srv_fd);
            return 1;
        }

        for (--i; i > -1; --i)
        {
            if (es[i].data.fd == srv_fd)
            {
                svHandler();
            }
            else
                //if (es[i].data.fd == cli_fd)
            {
                clHandler(&es[i]);
            }
        }
    }

    return 0;
}

