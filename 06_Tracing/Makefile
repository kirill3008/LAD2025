CC = gcc
CFLAGS = -Wall -Wextra -g
TEST_DIR = test_dir

all: move libprotect.so

move: main.c
	$(CC) $(CFLAGS) -o move main.c

libprotect.so: protect_lib.c
	$(CC) $(CFLAGS) -shared -fPIC -o libprotect.so protect_lib.c -ldl

prepare_tests:
	mkdir -p $(TEST_DIR)

test_basic: move prepare_tests
	@./move > /dev/null 2>&1 || test $$? -eq 1
	@echo "PASSED: Usage error test"
	@echo "test data" > "$(TEST_DIR)/samefile.txt"
	@./move "$(TEST_DIR)/samefile.txt" "$(TEST_DIR)/samefile.txt" > /dev/null 2>&1 || test $$? -eq 1
	@echo "PASSED: Same file test"
	@echo "test data" > "$(TEST_DIR)/regular_file.txt"
	@./move "$(TEST_DIR)/regular_file.txt" "$(TEST_DIR)/moved_regular_file.txt" > /dev/null 2>&1
	@test -f "$(TEST_DIR)/moved_regular_file.txt" && test ! -f "$(TEST_DIR)/regular_file.txt" && echo "PASSED: File moved successfully" || (echo "FAILED: File move operation" && exit 1)

test_stat_error: move prepare_tests
	@strace -P "$(TEST_DIR)/nonexistent.txt" -e inject=stat,lstat,fstat,stat64,lstat64,fstat64:error=ENOENT ./move "$(TEST_DIR)/nonexistent.txt" "$(TEST_DIR)/dest.txt" > /dev/null 2>&1 || test $$? -eq 9
	@echo "PASSED: Stat failure test"

test_open_source_error: move prepare_tests
	@echo "test data" > "$(TEST_DIR)/source.txt"
	@strace -P "$(TEST_DIR)/source.txt" -e inject=open,openat:error=EACCES ./move "$(TEST_DIR)/source.txt" "$(TEST_DIR)/dest2.txt" > /dev/null 2>&1 || test $$? -eq 2
	@test -f "$(TEST_DIR)/source.txt" && echo "PASSED: Open source failure test - source preserved" || (echo "FAILED: Source file unexpectedly removed" && exit 1)

test_open_dest_error: move prepare_tests
	@echo "test data" > "$(TEST_DIR)/source.txt"
	@strace -P "$(TEST_DIR)/dest3.txt" -e inject=open,openat:error=EACCES ./move "$(TEST_DIR)/source.txt" "$(TEST_DIR)/dest3.txt" > /dev/null 2>&1 || test $$? -eq 3
	@test -f "$(TEST_DIR)/source.txt" && echo "PASSED: Open destination failure test - source preserved" || (echo "FAILED: Source file unexpectedly removed" && exit 1)

test_read_error: move prepare_tests
	@echo "test data" > "$(TEST_DIR)/source.txt"
	@strace -e inject=read:error=EIO:when=3 ./move "$(TEST_DIR)/source.txt" "$(TEST_DIR)/dest4.txt" > /dev/null 2>&1 || test $$? -eq 4
	@test -f "$(TEST_DIR)/source.txt" && echo "PASSED: Read source failure test - source preserved" || (echo "FAILED: Source file unexpectedly removed" && exit 1)

test_write_error_strace: move prepare_tests
	@echo "test data" > "$(TEST_DIR)/source.txt"
	@strace -e inject=write:error=EIO:when=1 ./move "$(TEST_DIR)/source.txt" "$(TEST_DIR)/dest5.txt" > /dev/null 2>&1 || test $$? -eq 5
	@test -f "$(TEST_DIR)/source.txt" && echo "PASSED: Write destination failure test - source preserved" || (echo "FAILED: Source file unexpectedly removed" && exit 1)
	@test ! -f "$(TEST_DIR)/dest5.txt" && echo "PASSED: Write failure test - corrupted dest removed" || (echo "FAILED: Corrupted destination file exists" && exit 1)

# NOTE: Close error injection test removed
# File descriptors are assigned by the OS non-deterministically, and library
# loading can add unpredictable close() calls, making reliable fault injection
# for close() impractical without modifying the source program.

test_remove_error: move prepare_tests
	@echo "test data" > "$(TEST_DIR)/source.txt"
	@strace -P "$(TEST_DIR)/source.txt" -e inject=unlink,unlinkat:error=EACCES ./move "$(TEST_DIR)/source.txt" "$(TEST_DIR)/dest7.txt" > /dev/null 2>&1 || test $$? -eq 7
	@test -f "$(TEST_DIR)/source.txt" && test -f "$(TEST_DIR)/dest7.txt" && echo "PASSED: Remove source failure test - both files exist" || (echo "FAILED: Files state is incorrect" && exit 1)

test_protect: move libprotect.so prepare_tests
	@echo "protected data" > "$(TEST_DIR)/file_to_PROTECT.txt"
	@LD_PRELOAD=./libprotect.so ./move "$(TEST_DIR)/file_to_PROTECT.txt" "$(TEST_DIR)/dest8.txt" > /dev/null 2>&1 || true
	@test -f "$(TEST_DIR)/file_to_PROTECT.txt" && echo "PASSED: Protected file not removed" || (echo "FAILED: Protected file was removed" && exit 1)

test_protect_normal: move libprotect.so prepare_tests
	@echo "normal data" > "$(TEST_DIR)/normal_file.txt"
	@LD_PRELOAD=./libprotect.so ./move "$(TEST_DIR)/normal_file.txt" "$(TEST_DIR)/moved_normal.txt" > /dev/null 2>&1
	@test ! -f "$(TEST_DIR)/normal_file.txt" && test -f "$(TEST_DIR)/moved_normal.txt" && echo "PASSED: Normal file moved successfully with LD_PRELOAD" || (echo "FAILED: Normal file not moved correctly with LD_PRELOAD active" && exit 1)

# Run all tests
test: test_basic test_stat_error test_open_source_error test_open_dest_error test_read_error test_write_error_strace test_remove_error test_protect test_protect_normal
	@echo "\nAll tests completed successfully!"

clean:
	rm -f move libprotect.so *.o core
	rm -rf $(TEST_DIR)

.PHONY: all clean test prepare_tests test_basic test_stat_error test_open_source_error test_open_dest_error test_read_error test_write_error_strace test_remove_error test_protect test_protect_normal
	@echo "\nTesting successful move operation..."

