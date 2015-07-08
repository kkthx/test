#ifndef _USERS_H
#define _USERS_H

#include "defs.h"

typedef struct
{
	int id;
	char username[BUF_SIZE];
}
user;

void login(user *users, int id, char *recv_buf);
int delete_user(user *users, char *username);
int delete_user_id(user *users, int id);
void print_users(user *users);
void user_list(user *users, int id);
void send_message(user *users, int id, char *recv_buf);
void send_data(int *fd, char *send_data);


#endif
