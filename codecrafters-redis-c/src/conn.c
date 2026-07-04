/*
  TODO:
  - There is no error handling mode. Just use assert.
  - For every new string, we reallocation every time. What we should do is
  allocating once, and re-read into the buffer.
  - Normalize command into lower case - DONE
  - Make debug log in different levels.
  - Make utils function for response with simple string.
  - Add mutex.
*/

#include "conn.h"
#include "htable.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

CmdHandler handlers[] = {
    {.cmd = "ping", .callback = conn_callback_ping},
    {.cmd = "echo", .callback = conn_callback_echo},
    {.cmd = "set", .callback = conncb_set},
    {.cmd = "get", .callback = conncb_get},
};

void *handleConn(void *args) {
  int socket_fd = ((ConnArgs *)args)->socketfd;
  Manager *manager = ((ConnArgs *)args)->manager;
  ConnHandle *handle = conn_handle_init(socket_fd, manager);
  size_t cmd_count = sizeof(handlers) / sizeof(CmdHandler);

  while (1) {
    int argv_len = conn_read_int(handle, '*');
    if (argv_len == -1)
      break;
    printf("argv_len cmd: %d\n", argv_len);

    char *cmd = conn_read_bulk_str(handle);
    for (int i = 0; i < strlen(cmd); i++)
      cmd[i] = tolower(cmd[i]);
    printf("receive cmd: %s\n", cmd);
    Callback callback = conn_callback_ping;
    for (int i = 0; i < cmd_count; i++) {
      if (strcmp(cmd, handlers[i].cmd) == 0) {
        callback = handlers[i].callback;
        break;
      }
    }

    callback(handle, argv_len);
    free(cmd);
  }

  conn_handle_free(handle);
  close(socket_fd);
  free(args);
  printf("clean up: %d\n", handle->socket_fd);
  return NULL;
}

ConnHandle *conn_handle_init(int socket_fd, Manager *manager) {
  ConnHandle *re = malloc(sizeof(*re));
  assert(re);
  memset(re, 0, sizeof(*re));
  re->socket_fd = socket_fd;
  re->manager = manager;

  printf("Client connected: fd=%d\n", socket_fd);
  return re;
}

void conn_handle_free(ConnHandle *self) {
  if (self->tmp != NULL)
    free(self->tmp);
  free(self);
}

char *conn_read_token(ConnHandle *self) {
  int re_i = -1;
  while (1) {
    if (self->tmp != NULL) {
      printf("existing buf: pos(%d) - size(%d) - value[%s]\n", self->i,
             self->size, self->tmp);
      for (int i = self->i; i < self->size - 1; i++) {
        if (self->tmp[i] == '\r' && self->tmp[i + 1] == '\n') {
          re_i = i;
          goto found_token;
        }
      }
    }

    // read into buf then copy it into tmp
    memset(self->buf, 0, MAX_CHAR_PER_READ + 1);
    ssize_t valread = read(self->socket_fd, self->buf, MAX_CHAR_PER_READ);
    if (valread == 0) {
      printf("Connection exited: fd=%d\n", self->socket_fd);
      return NULL;
    }
    if (valread == -1) {
      printf("Accept failed: err=%s\n", strerror(errno));
      return NULL;
    }

    int len = strlen(self->buf);
    printf("read %d bytes - data = [%s]\n", len, self->buf);

    if (self->tmp != NULL) {
      self->tmp = realloc(self->tmp, self->size + len);
      memset(self->tmp + self->size, 0, len);
      strcpy(self->tmp + self->size - 1, self->buf);
      self->size = self->size + len;
    } else {
      self->tmp = calloc(len + 1, 1);
      strcpy(self->tmp, self->buf);
      self->size = len + 1;
    }
  }

  char *re;
  int re_len;
found_token:
  re_len = re_i - self->i;
  re = calloc(re_len, 1);
  memcpy(re, self->tmp + self->i, re_len);
  self->i += re_len + 2; // +2 to skip /r/n

  printf("read token: %s\n", re);
  return re;
}

int conn_read_int(ConnHandle *self, char leadingCh) {
  char *t1 = conn_read_token(self);
  if (t1 == NULL)
    return -1;
  assert(t1[0] == leadingCh);
  int re = atoi(&t1[1]);
  free(t1);
  return re;
}

char *conn_read_bulk_str(ConnHandle *self) {
  int size = conn_read_int(self, '$');
  char *t2 = conn_read_token(self);
  assert(strlen(t2) == size);
  return t2;
}

void conn_write_bulk_str(ConnHandle *self, char *in, int size) {
  int re = send(self->socket_fd, "$", 1, 0);
  assert(re != -1);
  char str[1024];
  re = sprintf(str, "%d", size);
  assert(re != -1);
  re = send(self->socket_fd, str, strlen(str), 0);
  assert(re != -1);
  re = send(self->socket_fd, "\r\n", 2, 0);
  assert(re != -1);

  re = send(self->socket_fd, in, size, 0);
  assert(re != -1);
  re = send(self->socket_fd, "\r\n", 2, 0);
  assert(re != -1);
}

void conn_callback_ping(ConnHandle *self, int size) {
  assert(size == 1);

  char *hello = "+PONG\r\n";
  int re = send(self->socket_fd, hello, strlen(hello), 0);
  assert(re != -1);
}

void conn_callback_echo(ConnHandle *self, int size) {
  assert(size == 2);

  char *msg = conn_read_bulk_str(self);
  printf("command=echo msg='%s'\n", msg);
  conn_write_bulk_str(self, msg, strlen(msg));

  free(msg);
}

void conncb_set(ConnHandle *self, int size) {
  char *key = conn_read_bulk_str(self);
  char *value = conn_read_bulk_str(self);

  map_insert(self->manager->map, key, value);

  char *hello = "+OK\r\n";
  int re = send(self->socket_fd, hello, strlen(hello), 0);
  assert(re != -1);
}

void conncb_get(ConnHandle *self, int size) {
  assert(size == 2);

  char *key = conn_read_bulk_str(self);

  char *value = map_get(self->manager->map, key);
  if (value == NULL) {
    char *hello = "$-1\r\n";
    int re = send(self->socket_fd, hello, strlen(hello), 0);
    assert(re != -1);
    return;
  }

  conn_write_bulk_str(self, value, strlen(value));
  free(key);
  free(value);
}
