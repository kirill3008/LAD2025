#include "roman.h"
#include <string.h>

/**
 * @brief Private lookup table for Roman numeral conversions
 * 
 * This array maps integers 1-100 to their Roman numeral representations.
 * The array is static to keep it internal to this translation unit.
 */
static const char *ROMAN_NUMERALS[100] = {
    "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
    "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
    "XXI", "XXII", "XXIII", "XXIV", "XXV", "XXVI", "XXVII", "XXVIII", "XXIX", "XXX",
    "XXXI", "XXXII", "XXXIII", "XXXIV", "XXXV", "XXXVI", "XXXVII", "XXXVIII", "XXXIX", "XL",
    "XLI", "XLII", "XLIII", "XLIV", "XLV", "XLVI", "XLVII", "XLVIII", "XLIX", "L",
    "LI", "LII", "LIII", "LIV", "LV", "LVI", "LVII", "LVIII", "LIX", "LX",
    "LXI", "LXII", "LXIII", "LXIV", "LXV", "LXVI", "LXVII", "LXVIII", "LXIX", "LXX",
    "LXXI", "LXXII", "LXXIII", "LXXIV", "LXXV", "LXXVI", "LXXVII", "LXXVIII", "LXXIX", "LXXX",
    "LXXXI", "LXXXII", "LXXXIII", "LXXXIV", "LXXXV", "LXXXVI", "LXXXVII", "LXXXVIII", "LXXXIX", "XC",
    "XCI", "XCII", "XCIII", "XCIV", "XCV", "XCVI", "XCVII", "XCVIII", "XCIX", "C"
};

const char *roman_int_to_roman(int num) {
    if (num < ROMAN_MIN_NUMBER || num > ROMAN_MAX_NUMBER) {
        return NULL;
    }
    return ROMAN_NUMERALS[num - 1];
}

int roman_roman_to_int(const char *roman) {
    if (!roman) return -1;
    
    char upper_roman[10];
    strncpy(upper_roman, roman, sizeof(upper_roman) - 1);
    upper_roman[sizeof(upper_roman) - 1] = '\0';
    
    for (char *p = upper_roman; *p; p++) {
        if (*p >= 'a' && *p <= 'z') {
            *p = *p - 'a' + 'A';
        }
    }
    
    for (int i = 0; i < ROMAN_MAX_NUMBER; i++) {
        if (strcmp(upper_roman, ROMAN_NUMERALS[i]) == 0) {
            return i + 1;
        }
    }
    
    return -1; // Not found
}
