CC = cc
CFLAGS = -Wall -Wextra -fPIC -std=c11 -g

all: libDateTime.so libRandom.so libString.so libIO.so

libDateTime.so: DateTime.c
	$(CC) $(CFLAGS) -shared -o libDateTime.so DateTime.c

libRandom.so: Random.c
	$(CC) $(CFLAGS) -shared -o libRandom.so Random.c

libString.so: String.c
	$(CC) $(CFLAGS) -shared -o libString.so String.c

libIO.so: IO.c
	$(CC) $(CFLAGS) -shared -o libIO.so IO.c

clean:
	rm -f libDateTime.so libRandom.so libString.so libIO.so

.PHONY: all clean
