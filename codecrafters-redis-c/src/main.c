/*
  TODO:
  - There is no error handling mode. Just use assert.
  - For every new string, we reallocation every time. What we should do is
  allocating once, and re-read into the buffer.
  - Add mutex.
  - Understand how Claude used Valgrind to find the memory error
  - Learn to do fuzz test for the table
  - Learn to do algorithm for hmap
*/

#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "conn.h"
#include "storage.h"
#include "utils.h"

int main() {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  logging(LOG_LEVEL_INFO, "Start! -  current_ts: %ld\n", current_ts());

  // server_fd contains the id to the file descriptor in Unix system.
  // It lives in /prod/<pid>/fd/<server_fd>
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    logging(LOG_LEVEL_WARN, "Socket creation failed: %s...\n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    logging(LOG_LEVEL_WARN, "SO_REUSEADDR failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr = {
      .sin_family = AF_INET,
      .sin_port = htons(6379),
      .sin_addr = {htonl(INADDR_ANY)},
  };

  if (bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    logging(LOG_LEVEL_WARN, "Bind failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    logging(LOG_LEVEL_WARN, "Listen failed: %s \n", strerror(errno));
    exit(EXIT_FAILURE);
  }

  Storage *storage = storage_init();

  logging(LOG_LEVEL_INFO, "Waiting for a client to connect...\n");

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  while (1) {
    int conn_fd =
        accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (conn_fd < 0) {
      logging(LOG_LEVEL_WARN, "Accept failed: %s \n", strerror(errno));
      exit(EXIT_FAILURE);
    }

    ConnArgs *args = malloc(sizeof(*args));
    args->fd = conn_fd;
    args->storage = storage;

    pthread_t thread1;
    pthread_create(&thread1, NULL, handleConn, (void *)args);
    pthread_detach(thread1);
  }
  close(server_fd);
  return 0;
}
