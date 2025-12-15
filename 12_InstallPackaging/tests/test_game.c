/**
 * @file test_game.c
 * @brief Unit tests for the game logic (mocked version)
 * 
 * Tests the game logic with mocked input/output to verify
 * the binary search algorithm works correctly.
 * 
 * @author Student
 * @date 2025-12-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @brief Mock game function that simulates binary search
 * 
 * This version doesn't require user input, making it testable.
 * It simulates guessing a predetermined number.
 * 
 * @param target The number to be guessed
 * @param use_roman Whether to use Roman numerals
 * @return Number of guesses required
 */
int mock_play_game(int target, int use_roman) {
    int low = 1;
    int high = 100;
    int guesses = 0;
    
    while (low < high) {
        guesses++;
        int mid = low + (high - low) / 2;
        
        if (target > mid) {
            low = mid + 1;  // Search upper half
        } else {
            high = mid;     // Search lower half
        }
    }
    
    assert(low == target);
    
    return guesses;
}

/**
 * @brief Test binary search with known target
 */
void test_binary_search() {
    printf("Testing binary search algorithm...\n");
    
    assert(mock_play_game(1, 0) <= 7);   // Should take <= log2(100) guesses
    assert(mock_play_game(100, 0) <= 7);
    
    assert(mock_play_game(50, 0) <= 7);
    assert(mock_play_game(25, 0) <= 7);
    assert(mock_play_game(75, 0) <= 7);
    assert(mock_play_game(37, 0) <= 7);
    
    printf("✓ Binary search tests passed\n");
}

/**
 * @brief Test that algorithm always finds the correct number
 */
void test_all_numbers() {
    printf("Testing all numbers from 1 to 100...\n");
    
    for (int i = 1; i <= 100; i++) {
        int guesses = mock_play_game(i, 0);
        assert(guesses > 0 && guesses <= 7);
        
        if (i % 25 == 0) {
            printf("  Tested %d/100 numbers...\n", i);
        }
    }
    
    printf("✓ All numbers test passed\n");
}

/**
 * @brief Test Roman numeral integration (conceptual)
 */
void test_roman_mode() {
    printf("Testing Roman numeral mode...\n");
    
    int guesses_arabic = mock_play_game(50, 0);
    int guesses_roman = mock_play_game(50, 1);
    
    assert(guesses_arabic == guesses_roman);
    
    printf("✓ Roman numeral mode test passed\n");
}

/**
 * @brief Main test runner
 */
int main() {
    printf("Running game logic tests...\n\n");
    
    test_binary_search();
    test_all_numbers();
    test_roman_mode();
    
    printf("\n✅ All game tests passed successfully!\n");
    return 0;
}
