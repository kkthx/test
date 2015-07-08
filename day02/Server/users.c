#include "users.h"
#include "defs.h"


int add_user(user *users, int id, char *username)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        //user exists
        if (strcmp(username, users[i].username) == 0)
		{
			return 1;
		}

        //found free place, adding user
		if (users[i].id == -1)
		{
            strcpy(users[i].username, username);
            users[i].id = id;
            return 0;
		}
	}

    //too many users connected
    return -1;
}

int delete_user(user *users, char *username)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        //user exists, delete then
        if (strcmp(username, users[i].username) == 0)
		{
            memcpy(users[i].username, 0, 1);
            users[i].id = -1;
			return 0;
		}
	}

    //user not found
    return 1;
}

void print_users(user *users)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        printf("%02d: %02d %s\n", i, users[i].id, users[i].username);
    }
}

