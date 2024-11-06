CC=gcc
CFLAGS=-Wall -Wextra -ggdb -std=gnu99
SRC=arith.c circ.c client.c hosts.c parse.c pre.c online.c msg.c com.c
OBJ=$(SRC:.c=.o)
LIBS=-lcrypto

all:		client

.SUFFIXES: 	.c .o
.c.o:
		$(CC) -c $(CFLAGS) $*.c

client: $(OBJ)
		$(CC) $(LIBS) -o slows $(OBJ)

clean:
		rm -f *.o slows
