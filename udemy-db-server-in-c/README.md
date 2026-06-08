# Coding a DB tree in C
- Ref: https://www.udemy.com/course/coding-a-database-server-in-c/learn/lecture/47875821#overview

## What is this:
- A cache server like redis

## How to the prototcol works
- `SELECT /Users/john`
- `CREATE /Users/login`
- `INSERT /Users/john foobar`
- Note:
  - Max command size is 255.

## Data structure:
- organize into folder (like `/Users/quangnd` and `Users`), this also call Node.
- Each node points to its parent folder (north), while the north of root node is itself.
- Each node contains several leaves. They are linked like a linked list (west <-> east)

## How to test:
- Build and run
```sh
make
./cache22
```

- Open another terminal
```sh
telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
100 conected to cache 22 server
```
## Modification from the course:
- I use (char *) instead of (int8 *) b/c I don't care about creating my own string datastructure.
- I use memset, instead of writing zeros.

=> The main reason is learning C, not creating everything from scatch.


### Questions:
- In the makefile explain `make tree` and `make tree.o`
- It looks like each node can have 1 child (west)
