#include "users.h"
#include "defs.h"

void login(user *users, int id, char *recv_buf, char *send_buf)
{
    if (strlen(recv_buf) > 2 && recv_buf[0] == '2' && recv_buf[1] == '.')
    {
        char *username = malloc(sizeof(char)*strlen(recv_buf));
        memset(username, 0, strlen(recv_buf));
        memcpy(username, recv_buf+2, strlen(recv_buf)-2);

        strcpy(send_buf, "1.1.Too many users connected");

        for (int i=0; i<MAXUSERS; ++i)
        {
            if (strcmp(username, users[i].username) == 0 || id == users[i].id)
            {
                strcpy(send_buf, "1.1.User already logged in");
                break;
            }

            //found free place, adding user
            if (users[i].id == -1)
            {
                strcpy(users[i].username, username);
                users[i].id = id;
                strcpy(send_buf, "1.0");
                break;
            }
        }

        free(username);
    }
    else
    {
        strcpy(send_buf, "1.1.No user name set");
    }

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
    printf("userlist: \n");
    for (int i=0; i<MAXUSERS; ++i)
    {
        printf("%02d: %02d %s\n", i, users[i].id, users[i].username);
    }
}

void user_list(user *users, char *send_buf)
{
    char userlist[BUF_SIZE];
    strcpy(userlist, "7");
    for (int i=0; i<MAXUSERS; ++i)
    {
        if (users[i].id != -1)
        {
            strcat(userlist, ".");
            strcat(userlist, users[i].username);
        }
    }
    strcpy(send_buf, userlist);
}
