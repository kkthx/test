#ifndef _USERS_H
#define _USERS_H

#include "defs.h"

typedef struct
{
	int id;
	char username[BUF_SIZE];
}
user;

void login(user *users, int id, char *recv_buf, char *send_buf);
int delete_user(user *users, char *username);
void print_users(user *users);
void user_list(user *users, char *send_buf);


#endif
