CC = cc
CFLAGS = -Wall -Wextra -fPIC -std=c11 -g -I. -D_POSIX_C_SOURCE=200809L

NATIVE_HDR = native.h

all: libDateTime.so libTime.so libRandom.so libString.so libIO.so libMath.so libColor.so libcolor.so

libDateTime.so: DateTime.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libDateTime.so DateTime.c

libTime.so: Time.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libTime.so Time.c

libRandom.so: Random.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libRandom.so Random.c

libString.so: String.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libString.so String.c

libIO.so: IO.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libIO.so IO.c

libMath.so: Math.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libMath.so Math.c -lm

libColor.so: Color.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libColor.so Color.c

libcolor.so: Color.c $(NATIVE_HDR)
	$(CC) $(CFLAGS) -shared -o libcolor.so Color.c

clean:
	rm -f libDateTime.so libTime.so libRandom.so libString.so libIO.so libMath.so libColor.so libcolor.so

.PHONY: all clean
