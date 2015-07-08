#ifndef _MAIN_H
#define _MAIN_H
#include <stdio.h>


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <signal.h>


#include "defs.h"
#include "users.h"



void closeSockets(int);
int svHandler(void);
int clHandler(struct epoll_event*);



#endif
