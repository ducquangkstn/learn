/* cache22.c*/

#include "cache22.h"


bool s_continuation = true;
bool c_continuation = true;

int init_server(uint16 port) {
  // AF_INET is IPv4 Internet protocl
  // SOCK_STREAM = tcp
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(server_fd);

  // Forcefully attaching socket to the port 8080
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET,
    SO_REUSEADDR | SO_REUSEPORT, &opt,
    sizeof(opt))) {
    assert_perror(errno);
  }


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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
void child_loop(Conn_handle* self) {
  char buf[256];
  memset(buf, 0, 256);// last byte will always be 0

  int re = read(self->sock_fd, buf, 255);
  assert(re != -1 && re != 0); // TODO: what should we do if re == 0 (EOF)

  char cmd[256], folder[256], args[256];
  memset(cmd, 0, 256);  memset(folder, 0, 256);memset(args, 0, 256);

  char* p = buf;
  for (;(*p) && (*p != '\n') && (*p != '\r') && (*p != ' ');p++);
  bool finished = (*p == 0 || *p == '\n' || *p == '\r');
  *p = 0; // trick: set the space into \0, so that strncopy will stop there.
  strncpy(cmd, buf, 255);
  if (finished) goto done_parsing;

  char* tmp = ++p;
  for (;(*p) && (*p != '\n') && (*p != '\r') && (*p != ' ');p++);
  finished = (*p == 0 || *p == '\n' || *p == '\r');
  *p = 0;
  strncpy(folder, tmp, 255);
  if (finished) goto done_parsing;

  tmp = ++p;
  for (;(*p) && (*p != '\n') && (*p != '\r');p++);
  if ((*p == '\r') || (*p == '\n')) *p = 0;
  strncpy(args, tmp, 255);


done_parsing:
  dprintf(self->sock_fd, "cmd:\t%s\n", cmd);
  dprintf(self->sock_fd, "folder:\t%s\n", folder);
  dprintf(self->sock_fd, "args:\t'%s'\n", args);
  return;
}
#pragma GCC diagnostic pop


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