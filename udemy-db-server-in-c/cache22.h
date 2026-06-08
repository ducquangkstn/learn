/* cache22.h */
#ifndef CACHE22
#define CACHE22

#define _GNU_SOURCE // only compatible to linux
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#define HOST "127.0.0.1"
#define PORT "8080"

typedef unsigned int uint32;
typedef unsigned short int uint16;

typedef struct s_conn_handle {
  int sock_fd;
  uint16 port;
  char ip[16];
} Conn_handle;

typedef uint32(*Callback) (Conn_handle* self, char* path, char* args);
typedef struct s_cmdhandler {
  char* cmd;
  Callback handler;
} CmdHandler;

Callback get_cmd(char* cmd);


void child_loop(Conn_handle* self);
void main_loop(int server_fd);
int init_server(uint16 port);


#endif