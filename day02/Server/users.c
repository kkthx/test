#include "users.h"
#include "defs.h"

void login(user *users, int id, char *recv_buf)
{
    char *send_buf = malloc(BUF_SIZE*sizeof(char)+1);
    memset(send_buf, 0, BUF_SIZE+1);

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

    send_data(id, send_buf);

    free(send_buf);
}


int delete_user(user *users, char *username)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        //user exists, delete then
        if (strcmp(username, users[i].username) == 0)
        {
            strcpy(users[i].username, "");
            users[i].id = -1;
            return 0;
        }
    }

    //user not found
    return 1;
}

int delete_user_id(user *users, int id)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        //user exists, delete then
        if (users[i].id == id)
        {
            strcpy(users[i].username, "");
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


void user_list(user *users, int id)
{
    char *send_buf = malloc(BUF_SIZE*sizeof(char)+1);
    memset(send_buf, 0, BUF_SIZE+1);

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

    send_data(id, send_buf);

    free(send_buf);
}


void send_message(user *users, int id, char *recv_buf)
{
    char *send_buf = malloc(BUF_SIZE*sizeof(char)+1);
    memset(send_buf, 0, BUF_SIZE+1);

    if (strlen(recv_buf) > 2 && (recv_buf[0] == '3' || recv_buf[0] == '5') && recv_buf[1] == '.')
    {
        char *buf = malloc(sizeof(char)*strlen(recv_buf));
        //char *message = malloc(sizeof(char)*strlen(recv_buf));

        memset(buf, 0, strlen(recv_buf));
        memcpy(buf, recv_buf+2, strlen(recv_buf)-2);

        if (strchr(buf, '.'))
        {
            char *receiver;
            char *message;
            receiver = strtok(buf, ".");
            message = strtok(NULL, ".");


            char *sender = get_user_name(users, id);
            strcpy(send_buf, "4.");
            strcat(send_buf, sender);
            strcat(send_buf, ".");
            strcat(send_buf, message);

            if (recv_buf[0] == '3') //MessageTo
            {
                // check if receiver is logged in
                // get sender username string
                // send message to user

                if (get_user_id(users, receiver) != -1)
                {
                    send_data(get_user_id(users, receiver), send_buf);
                }
                else
                {
                    strcpy(send_buf, "1.1.User is not logged in");
                    send_data(id, send_buf);
                }
            }
            else //Broadcast
            {
                // for over all users where id != -1 and id != sender
                int sender_id = get_user_id(users, sender);
                for (int i=0; i<MAXUSERS; ++i)
                {
                    if (users[i].id != -1 && users[i].id != sender_id)
                    {
                         send_data(users[i].id, send_buf);
                    }
                }

            }

        }
        else
        {
            strcpy(send_buf, "1.1.Unknown format");
            send_data(id, send_buf);
        }

        free(buf);
    }
    else
    {
        strcpy(send_buf, "1.1.No user name set");
        send_data(id, send_buf);
    }

    free(send_buf);
}

void send_data(int fd, char *send_data)
{
    size_t msg_len = strlen(send_data)+1;
    write(fd, &msg_len, sizeof(size_t));
    write(fd, send_data, msg_len);
}

int get_user_id(user *users, char *username)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        if (strcmp(username, users[i].username) == 0)
        {
            return users[i].id;
        }
    }

    return -1;
}

char* get_user_name(user *users, int id)
{
    for (int i=0; i<MAXUSERS; ++i)
    {
        if (users[i].id == id)
        {
            return users[i].username;
        }
    }

    return NULL;
}




