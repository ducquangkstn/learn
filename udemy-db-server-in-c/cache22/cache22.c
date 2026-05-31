/* cache22.c*/

#include "cache22.h"


bool s_continuation = true;
bool c_continuation = true;

int init_server(uint16 port) {
  // AF_INET is IPv4 Internet protocl
  // SOCK_STREAM = tcp
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(server_fd);

  struct sockaddr_in sock = {
    .sin_family = AF_INET,
    .sin_port = htons(port),
    .sin_addr = {.s_addr = inet_addr(HOST)}
  };

  errno = 0;
  if (bind(server_fd, (struct sockaddr*)&sock, sizeof(sock)))
    assert_perror(errno);


  errno = 0;
  if (listen(server_fd, 20))
    assert_perror(errno);

  printf("Server is lisetning on %d\n", port);
  return server_fd;
}

void child_loop(Conn_handle* self) {
  sleep(1);
}

void main_loop(int server_fd) {

  struct sockaddr_in cli;
  unit32 cli_len = sizeof(cli);

  int sock_fd = accept(server_fd, (struct sockaddr*)&cli, &cli_len);
  if (sock_fd == -1) {
    sleep(1);
    return; // handle the error here.
  }

  uint16 cli_port = htons((int)cli.sin_port);
  char* ip = inet_ntoa(cli.sin_addr);

  printf("client connection from %s:%d\n", ip, cli_port);

  Conn_handle* handle = malloc(sizeof(*handle));
  assert(handle);
  memset(handle, 0, sizeof(*handle));
  handle->sock_fd = sock_fd;
  handle->port = cli_port;
  strncpy(handle->ip, ip, 15);

  pid_t pid = fork(); // create child process
  if (pid) {
    free(handle);
    return;
  }

  dprintf(sock_fd, "100 conected to cache 22 server\n");
  while (c_continuation)
    child_loop(handle);

  free(handle);
  close(sock_fd);
}

int main(int argc, char* argv[]) {
  char* port_str;
  if (argc < 2)
    port_str = PORT;
  else
    port_str = argv[1];

  uint16 port = atoi(port_str);
  int server_fd = init_server(port);

  while (s_continuation)
    main_loop(server_fd);

  close(server_fd);
  return 0;
}