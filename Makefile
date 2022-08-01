CFLAGS=-Wall -Wpedantic -I/usr/include/postgresql
LDLIBS=-lpq -ljson-c

BIN=cloud

all: $(BIN)

clean:
	rm -f *.o $(BIN)
