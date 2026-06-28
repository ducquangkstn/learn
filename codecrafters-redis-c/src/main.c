#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "conn.h"


int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible when running tests.
  printf("Logs from your program will appear here!\n");

  // server_fd contains the id to the file descriptor in Unix system.
  // It lives in /prod/<pid>/fd/<server_fd>
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    printf("Socket creation failed: %s...\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    printf("SO_REUSEADDR failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(6379),
    .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
    printf("Bind failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    printf("Listen failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  printf("Waiting for a client to connect...\n");

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  while (1) {
    int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (new_socket < 0) {
      printf("Accept failed: %s \n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    printf("Client connected 1: %d\n", new_socket);

    pthread_t thread1;
    pthread_create(&thread1, NULL, hanldeConn, (void*)&new_socket);

    pthread_detach(thread1);
  }
  close(server_fd);
  return 0;
}

