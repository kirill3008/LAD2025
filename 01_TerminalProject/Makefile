CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBS = -lncursesw

all: Show

Show: Show.c
	$(CC) $(CFLAGS) -o Show Show.c $(LIBS)

clean:
	rm -f Show

.PHONY: all clean
