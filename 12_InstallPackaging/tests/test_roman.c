/**
 * @file test_roman.c
 * @brief Unit tests for Roman numeral library
 * 
 * Tests the roman_int_to_roman() and roman_roman_to_int() functions
 * with various edge cases.
 * 
 * @author Student
 * @date 2025-12-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../lib/roman.h"

/**
 * @brief Test conversion from integer to Roman numerals
 */
void test_int_to_roman() {
    printf("Testing int_to_roman...\n");
    
    assert(strcmp(roman_int_to_roman(1), "I") == 0);
    assert(strcmp(roman_int_to_roman(5), "V") == 0);
    assert(strcmp(roman_int_to_roman(10), "X") == 0);
    assert(strcmp(roman_int_to_roman(50), "L") == 0);
    assert(strcmp(roman_int_to_roman(100), "C") == 0);
    assert(strcmp(roman_int_to_roman(4), "IV") == 0);
    assert(strcmp(roman_int_to_roman(9), "IX") == 0);
    assert(strcmp(roman_int_to_roman(49), "XLIX") == 0);
    assert(strcmp(roman_int_to_roman(99), "XCIX") == 0);
    
    assert(roman_int_to_roman(0) == NULL);
    assert(roman_int_to_roman(-1) == NULL);
    assert(roman_int_to_roman(101) == NULL);
    
    printf("✓ All int_to_roman tests passed\n");
}

/**
 * @brief Test conversion from Roman numerals to integers
 */
void test_roman_to_int() {
    printf("Testing roman_to_int...\n");
    
    assert(roman_roman_to_int("I") == 1);
    assert(roman_roman_to_int("V") == 5);
    assert(roman_roman_to_int("X") == 10);
    assert(roman_roman_to_int("L") == 50);
    assert(roman_roman_to_int("C") == 100);
    assert(roman_roman_to_int("IV") == 4);
    assert(roman_roman_to_int("IX") == 9);
    assert(roman_roman_to_int("XLIX") == 49);
    assert(roman_roman_to_int("XCIX") == 99);
    
    assert(roman_roman_to_int("i") == 1);
    assert(roman_roman_to_int("v") == 5);
    assert(roman_roman_to_int("x") == 10);
    
    assert(roman_roman_to_int(NULL) == -1);
    assert(roman_roman_to_int("") == -1);
    assert(roman_roman_to_int("ABC") == -1);
    assert(roman_roman_to_int("IIII") == -1);
    assert(roman_roman_to_int("IC") == -1);
    
    printf("✓ All roman_to_int tests passed\n");
}

/**
 * @brief Test round-trip conversion (int -> roman -> int)
 */
void test_round_trip() {
    printf("Testing round-trip conversion...\n");
    
    for (int i = 1; i <= 100; i++) {
        const char *roman = roman_int_to_roman(i);
        assert(roman != NULL);
        
        int result = roman_roman_to_int(roman);
        if (result != i) {
            printf("  Failed for i=%d: roman='%s', result=%d\n", i, roman, result);
        }
        assert(result == i);
    }
    
    printf("✓ All round-trip tests passed (1-100)\n");
}

/**
 * @brief Main test runner
 */
int main() {
    printf("Running Roman numeral library tests...\n\n");
    
    test_int_to_roman();
    test_roman_to_int();
    test_round_trip();
    
    printf("\n✅ All tests passed successfully!\n");
    return 0;
}
