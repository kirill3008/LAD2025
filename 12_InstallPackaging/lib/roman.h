#ifndef ROMAN_H
#define ROMAN_H

/** Range constants */
#define ROMAN_MIN_NUMBER 1
#define ROMAN_MAX_NUMBER 100

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert integer to Roman numeral
 * 
 * @param num Integer between 1 and 100
 * @return Roman numeral string or NULL if out of range
 * 
 * @warning Returns NULL if number is outside valid range (1-100)
 */
const char *roman_int_to_roman(int num);

/**
 * @brief Convert Roman numeral to integer
 * 
 * @param roman Null-terminated Roman numeral string
 * @return Integer value or -1 if invalid
 * 
 * Performs case-insensitive lookup. Valid range: I to C (1-100)
 */
int roman_roman_to_int(const char *roman);

#ifdef __cplusplus
}
#endif

#endif /* ROMAN_H */