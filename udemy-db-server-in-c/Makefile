flags=-Wall -O2 -std=c2x
ldflags=

all: clean cache22 tree

cache22: cache22.o
	cc ${flags} $^ -o $@ ${ldflags}

cache22.o: cache22.c
	cc ${flags} -c $^

tree: tree.o
	cc ${flags} $^ -o $@ ${ldflags}

tree.o: tree.c
	cc ${flags} -c $^

clean:
	rm -rf *.o cache22 tree