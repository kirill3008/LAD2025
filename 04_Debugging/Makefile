CC = gcc
CFLAGS = -O0 -g
BINARY = range

all: $(BINARY)

$(BINARY): range.c
	$(CC) $(CFLAGS) $< -o $@

test: $(BINARY) test1 test2
	@echo "All tests completed"

test1: $(BINARY)
	gdb --batch --quiet -x gdb_script1.txt ./$(BINARY) | grep '^@@@' > test1_output.txt
	@if [ -f test1_reference.txt ]; then \
		diff -u test1_reference.txt test1_output.txt && echo "Test 1 passed" || echo "Test 1 failed"; \
	else \
		echo "No reference file found for test 1"; \
	fi

test2: $(BINARY)
	gdb --batch --quiet -x gdb_script2.txt ./$(BINARY) | grep '^@@@' > test2_output.txt
	@if [ -f test2_reference.txt ]; then \
		diff -u test2_reference.txt test2_output.txt && echo "Test 2 passed" || echo "Test 2 failed"; \
	else \
		echo "No reference file found for test 2"; \
	fi

clean:
	rm -f $(BINARY) test1_output.txt test2_output.txt

.PHONY: all test test1 test2 clean
