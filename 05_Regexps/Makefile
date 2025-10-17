CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic

all: esub

esub: esub.c
	$(CC) $(CFLAGS) -o esub esub.c

# Clean generated files
clean:
	rm -f esub *.o test_out.txt test_err.txt sed_out.txt

# Tests comparing esub output with sed -E
test: esub
	@echo "Running tests..."
	@echo "Test 1: Basic substitution"
	@./esub "hello" "world" "hello there" > test_out.txt
	@echo "hello there" | sed -E 's/hello/world/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@echo "Test 2: Capture group substitution"
	@./esub "(h)ell(o)" "\\1-\\2" "hello there" > test_out.txt
	@echo "hello there" | sed -E 's/(h)ell(o)/\1-\2/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@echo "Test 3: Multiple capture groups"
	@./esub "([0-9]+)-([0-9]+)-([0-9]+)" "\\3.\\2.\\1" "Date: 2023-05-15" > test_out.txt
	@echo "Date: 2023-05-15" | sed -E 's/([0-9]+)-([0-9]+)-([0-9]+)/\3.\2.\1/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@echo "Test 4: Escaped backslash"
	@./esub "path" "path_\\\\replaced" "path/to/file" > test_out.txt
	@echo "path/to/file" | sed -E 's/path/path_\\replaced/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@echo "Test 5: No match"
	@./esub "xyz" "abc" "hello there" > test_out.txt
	@echo "hello there" | sed -E 's/xyz/abc/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@echo "Test 6: Complex regex"
	@./esub "([a-z]+)@([a-z]+)\\.([a-z]+)" "\\1 at \\2 dot \\3" "contact: user@example.com" > test_out.txt
	@echo "contact: user@example.com" | sed -E 's/([a-z]+)@([a-z]+)\.([a-z]+)/\1 at \2 dot \3/' > sed_out.txt
	@diff test_out.txt sed_out.txt && echo "✓ Passed" || echo "✗ Failed"

	@rm -f test_out.txt sed_out.txt

# Test with color output (visual inspection)
test-color: esub
	@echo "Testing colored output (visual inspection):"
	@./esub -c "([0-9]+)-([0-9]+)-([0-9]+)" "Date: \\1/\\2/\\3" "2023-05-15"
	@echo ""
	@echo "Testing color cycling with many capture groups:"
	@echo "Original: A1-B2-C3-D4-E5-F6-G7"
	@./esub -c "([A-Z][0-9])-([A-Z][0-9])-([A-Z][0-9])-([A-Z][0-9])-([A-Z][0-9])-([A-Z][0-9])-([A-Z][0-9])" "\\1 \\2 \\3 \\4 \\5 \\6 \\7" "A1-B2-C3-D4-E5-F6-G7"
	@echo "(Colors should cycle: RED → GREEN → YELLOW → BLUE → MAGENTA → CYAN → RED)"

# Test regex error diagnostics
test-errors: esub
	@echo "Testing regerror diagnostics:"
	@echo "---------------------"
	@echo "Test 1: Unclosed bracket error"
	@./esub "[a-z" "replacement" "test string" 2> test_err.txt || true
	@grep -q "Regex error:" test_err.txt && echo "✓ Proper regerror diagnostic" || echo "✗ Missing proper regerror message"
	@echo ""
	@echo "Test 2: Unclosed parenthesis error"
	@./esub "(hello" "replacement" "test string" 2> test_err.txt || true
	@grep -q "Regex error:" test_err.txt && echo "✓ Proper regerror diagnostic" || echo "✗ Missing proper regerror message"
	@echo ""
	@echo "Test 3: Reference to non-existent group"
	@./esub "hello" "\\1" "hello world" 2> test_err.txt || true
	@grep -q "Reference to non-existent group" test_err.txt && echo "✓ Proper error message" || echo "✗ Missing proper error message"
	@rm -f test_err.txt
	@echo "---------------------"

# Run all tests
check: test test-color test-errors

.PHONY: all clean test test-color test-errors check
