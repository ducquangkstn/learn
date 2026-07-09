#include "conn.h"
#include "htable.h"
#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

CmdHandler handlers[] = {
    {.cmd = "ping", .callback = conncb_ping},
    {.cmd = "echo", .callback = conncb_echo},
    {.cmd = "set", .callback = conncb_set},
    {.cmd = "get", .callback = conncb_get},
};

void *handleConn(void *args) {
  int fd = ((ConnArgs *)args)->fd;
  Storage *storage = ((ConnArgs *)args)->storage;
  ConnHandle *handle = conn_handle_init(fd, storage);
  size_t cmd_count = sizeof(handlers) / sizeof(CmdHandler);

  while (1) {
    int argv_len = conn_read_int(handle, '*');
    if (argv_len == -1)
      break;
    logging(LOG_LEVEL_DEBUG, "[fd=%d], argv_len cmd: %d\n", fd, argv_len);

    char *cmd = conn_read_bulk_str(handle);
    for (int i = 0; i < strlen(cmd); i++)
      cmd[i] = tolower(cmd[i]);
    logging(LOG_LEVEL_DEBUG, "[fd=%d] receive cmd: %s\n", fd, cmd);

    Callback callback = conncb_unknown;
    for (int i = 0; i < cmd_count; i++) {
      if (strcmp(cmd, handlers[i].cmd) == 0) {
        callback = handlers[i].callback;
        break;
      }
    }

    callback(handle, argv_len);
    free(cmd);
  }

  logging(LOG_LEVEL_INFO, "[fd=%d] clean up\n", handle->fd);

  conn_handle_free(handle);
  close(fd);
  free(args);
  return NULL;
}

ConnHandle *conn_handle_init(int fd, Storage *storage) {
  ConnHandle *re = malloc(sizeof(*re));
  assert(re);
  memset(re, 0, sizeof(*re));
  re->fd = fd;
  re->storage = storage;

  logging(LOG_LEVEL_INFO, "[fd=%d] Client connected\n", fd);
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
      logging(LOG_LEVEL_TRACE, "existing buf: pos(%d) - size(%d) - value[%s]\n",
              self->i, self->size, self->tmp);
      for (int i = self->i; i < self->size - 1; i++) {
        if (self->tmp[i] == '\r' && self->tmp[i + 1] == '\n') {
          re_i = i;
          goto found_token;
        }
      }
    }

    // read into buf then copy it into tmp
    memset(self->buf, 0, MAX_CHAR_PER_READ + 1);
    ssize_t valread = read(self->fd, self->buf, MAX_CHAR_PER_READ);
    if (valread == 0) {
      logging(LOG_LEVEL_INFO, "[fd=%d] EOF - Connection exited\n", self->fd);
      return NULL;
    }
    if (valread == -1) {
      logging(LOG_LEVEL_WARN, "[fd=%d] Accept failed: err=%s\n", self->fd,
              strerror(errno));
      return NULL;
    }

    int len = strlen(self->buf);
    logging(LOG_LEVEL_TRACE, "read %d bytes - data = [%s]\n", len, self->buf);

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

  logging(LOG_LEVEL_TRACE, "read token: %s\n", re);
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
  int re = send(self->fd, "$", 1, 0);
  assert(re != -1);
  char str[1024];
  re = sprintf(str, "%d", size);
  assert(re != -1);
  re = send(self->fd, str, strlen(str), 0);
  assert(re != -1);
  re = send(self->fd, "\r\n", 2, 0);
  assert(re != -1);

  re = send(self->fd, in, size, 0);
  assert(re != -1);
  re = send(self->fd, "\r\n", 2, 0);
  assert(re != -1);
}

void conncb_unknown(ConnHandle *self, int size) {
  conn_respond_simple_str(self, "+UNKNOWN\r\n");
}

void conncb_ping(ConnHandle *self, int size) {
  assert(size == 1);
  conn_respond_simple_str(self, "+PONG\r\n");
}

void conncb_echo(ConnHandle *self, int size) {
  assert(size == 2);

  char *msg = conn_read_bulk_str(self);
  logging(LOG_LEVEL_INFO, "[fd=%d] command=echo msg='%s'\n", self->fd, msg);
  conn_write_bulk_str(self, msg, strlen(msg));

  free(msg);
}

void conncb_set(ConnHandle *self, int size) {
  assert(size >= 3);
  char *key = conn_read_bulk_str(self);
  char *value = conn_read_bulk_str(self);

  Value *v = value_init_string(value);

  for (int i = 3; i < size; i++) {
    char *next = conn_read_bulk_str(self);
    if (strcmp(next, "EX") == 0 || strcmp(next, "PX") == 0) {
      int64_t durationinMs = (strcmp(next, "EX") == 0) ? 1000 : 1;
      char *next2 = conn_read_bulk_str(self);
      durationinMs *= atoi(next2);
      v->expiredInMs = current_ts() + durationinMs;
      i++;
      free(next2);
    }
    free(next);
  }

  logging(LOG_LEVEL_INFO, "set k=%s - v=%s - expiredTs = %ld\n", key, value,
          v->expiredInMs);

  map_insert(self->storage->map, key, v);

  conn_respond_simple_str(self, "+OK\r\n");

  free(key);
}

void conncb_get(ConnHandle *self, int size) {
  assert(size == 2);

  char *key = conn_read_bulk_str(self);

  Value *v = (Value *)map_get(self->storage->map, key);
  if (v == NULL) {
    char *hello = "$-1\r\n";
    int re = send(self->fd, hello, strlen(hello), 0);
    assert(re != -1);
    goto cleanup;
  }

  if (v->expiredInMs != -1 && v->expiredInMs < current_ts()) {
    char *hello = "$-1\r\n";
    int re = send(self->fd, hello, strlen(hello), 0);
    assert(re != -1);
    goto cleanup;
  }
  char *str = v->v;
  logging(LOG_LEVEL_INFO, "cache hit, value: %s\n", str);

  conn_write_bulk_str(self, str, strlen(str));

cleanup:
  free(key);
}
