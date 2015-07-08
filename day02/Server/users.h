#ifndef _USERS_H
#define _USERS_H

#include "defs.h"

typedef struct
{
	int id;
	char username[BUF_SIZE];
}
user;

int add_user(user *users, int id, char *username);
int delete_user(user *users, char *username);
void print_users(user *users);


#endif
