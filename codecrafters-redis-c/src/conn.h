#include <assert.h>
#include <stdint.h>

#ifndef CONNECTION
#define CONNECTION

void *handleConn(void *args);

#define MAX_CHAR_PER_READ 1024
#define DEBUG_MODE 1

// ConnHandle is the centre place for parsing everything
typedef struct s_conn_handle {
  char buf[MAX_CHAR_PER_READ + 1];
  char *tmp;
  int16_t size; // size of tmp
  int16_t i;    // pointer to the current position in the tmp
  int socket_fd;
  /* data */
} ConnHandle;

ConnHandle *conn_handle_init(int socket_fd);
void conn_handle_free(ConnHandle *self);

// read a token from the conneciton. Note that the redis protocol is using /r/n
// as delimiter hence we are using it to split a token. return a char*, receiver
// should release the buffer.
char *conn_read_token(ConnHandle *self);
int conn_read_int(ConnHandle *self, char leadingCh);
char *conn_read_bulk_str(ConnHandle *self);
void conn_write_bulk_str(ConnHandle *self, char *ch, int size);

void conn_callback_ping(ConnHandle *self, int size);
void conn_callback_echo(ConnHandle *self, int size);

typedef void (*Callback)(ConnHandle *self, int size);
typedef struct s_cmd_handler {
  char *cmd;
  Callback callback;
} CmdHandler;

#endif