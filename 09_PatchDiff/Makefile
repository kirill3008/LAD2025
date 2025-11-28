CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all run clean

all: maze maze_p1 maze_p2 maze_p3

# Generate patched versions
maze_p1.c: maze.c patch1.patch
	patch -o maze_p1.c maze.c patch1.patch

maze_p2.c: maze_p1.c patch2.patch
	patch -o maze_p2.c maze_p1.c patch2.patch

maze_p3.c: maze_p2.c patch3.patch
	patch -o maze_p3.c maze_p2.c patch3.patch

# Compile all programs
maze: maze.c
	$(CC) $(CFLAGS) -o $@ $<

maze_p1: maze_p1.c
	$(CC) $(CFLAGS) -o $@ $<

maze_p2: maze_p2.c
	$(CC) $(CFLAGS) -o $@ $<

maze_p3: maze_p3.c
	$(CC) $(CFLAGS) -o $@ $<

# Run all programs with example parameters
run: all
	@echo "\n--- Running original maze ---"
	./maze
	@echo "\n--- Running maze_p1 with size 4 ---"
	./maze_p1 4
	@echo "\n--- Running maze_p2 with characters '.#' and size 5 ---"
	./maze_p2 .# 5
	@echo "\n--- Running maze_p3 with seed 42, characters 'O#' and size 3 ---"
	./maze_p3 42 O# 3

# Clean generated files
clean:
	rm -f maze maze_p1 maze_p2 maze_p3 maze_p1.c maze_p2.c maze_p3.c
