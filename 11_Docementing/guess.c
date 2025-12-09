/**
 * @file guess.c
 * @brief Number guessing game with Roman numeral support
 * 
 * This program implements a binary search algorithm to guess a number 
 * between 1 and 100. It supports both Arabic and Roman numeral formats.
 *
 * @author Student
 * @date 2025-12-09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>
#include <getopt.h>

/** Localization macro */
#define _(STRING) gettext(STRING)

/** Program name for help and man pages */
#define PROGRAM_NAME "number-game"

/** Version string */
#define VERSION "1.0"

/** Range constants */
#define MIN_NUMBER 1
#define MAX_NUMBER 100

/**
 * @brief Array of Roman numeral equivalents for numbers 1-100
 * 
 * This lookup table provides Roman numeral representations for numbers
 * from 1 to 100. Used when -r flag is specified.
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

/**
 * @brief Convert integer to Roman numeral
 * 
 * @param num Integer between 1 and 100
 * @return Roman numeral string or NULL if out of range
 * 
 * @warning Will return NULL if number is outside valid range (1-100)
 */
const char *int_to_roman(int num) {
    if (num < MIN_NUMBER || num > MAX_NUMBER) {
        return NULL;
    }
    return ROMAN_NUMERALS[num - 1];
}

/**
 * @brief Convert Roman numeral to integer
 * 
 * @param roman Null-terminated Roman numeral string
 * @return Integer value or -1 if invalid
 * 
 * Performs case-insensitive lookup in the ROMAN_NUMERALS array.
 */
int roman_to_int(const char *roman) {
    if (!roman) return -1;
    
    // Convert to uppercase for case-insensitive comparison
    char upper_roman[10];
    strncpy(upper_roman, roman, sizeof(upper_roman) - 1);
    upper_roman[sizeof(upper_roman) - 1] = '\0';
    
    for (char *p = upper_roman; *p; p++) {
        if (*p >= 'a' && *p <= 'z') {
            *p = *p - 'a' + 'A';
        }
    }
    
    // Linear search through the lookup table
    for (int i = 0; i < MAX_NUMBER; i++) {
        if (strcmp(upper_roman, ROMAN_NUMERALS[i]) == 0) {
            return i + 1;
        }
    }
    
    return -1; // Not found
}

/**
 * @brief Print program usage information
 * 
 * @param program_name Name of the executable
 * @param lang Language code ('en' for English, 'ru' for Russian)
 */
void print_help(const char *program_name, const char *lang) {
    if (strcmp(lang, "ru") == 0) {
        printf("Использование: %s [ОПЦИИ]\n", program_name);
        printf("Бинарный поиск для угадывания числа от 1 до 100.\n\n");
        printf("Опции:\n");
        printf("  -r, --roman          Использовать римские числа вместо арабских\n");
        printf("  -h, --help           Показать эту справку и выйти\n");
        printf("  --help-md            Показать справку в формате Markdown\n");
        printf("  -v, --version        Показать версию и выйти\n");
        printf("\nПримеры:\n");
        printf("  %s                    # Игра с арабскими числами\n", program_name);
        printf("  %s -r                 # Игра с римскими числами\n", program_name);
    } else {
        printf("Usage: %s [OPTIONS]\n", program_name);
        printf("Binary search algorithm to guess a number between 1 and 100.\n\n");
        printf("Options:\n");
        printf("  -r, --roman          Use Roman numerals instead of Arabic\n");
        printf("  -h, --help           Show this help message and exit\n");
        printf("  --help-md            Show help in Markdown format\n");
        printf("  -v, --version        Show version information and exit\n");
        printf("\nExamples:\n");
        printf("  %s                    # Game with Arabic numerals\n", program_name);
        printf("  %s -r                 # Game with Roman numerals\n", program_name);
    }
}

/**
 * @brief Print help information in Markdown format
 * 
 * Generates comprehensive documentation in Markdown format suitable for
 * Doxygen processing and documentation generation.
 */
void print_help_md(void) {
    printf("# Number Guessing Game\n\n");
    printf("## Description\n\n");
    printf("`%s` is a command-line game that implements a binary search algorithm\n", PROGRAM_NAME);
    printf("to guess a number between 1 and 100 that the user is thinking of.\n\n");
    
    printf("## Features\n\n");
    printf("- Binary search algorithm for efficient number guessing\n");
    printf("- Support for both Arabic numerals (1-100) and Roman numerals (I-C)\n");
    printf("- Internationalization support (English and Russian)\n");
    printf("- Command-line options for customization\n\n");
    
    printf("## Usage\n\n");
    printf("```\n");
    printf("%s [OPTIONS]\n", PROGRAM_NAME);
    printf("```\n\n");
    
    printf("## Options\n\n");
    printf("| Option | Long Option | Description |\n");
    printf("|--------|-------------|-------------|\n");
    printf("| `-r` | `--roman` | Use Roman numerals instead of Arabic |\n");
    printf("| `-h` | `--help` | Show standard help message |\n");
    printf("|  | `--help-md` | Show help in Markdown format |\n");
    printf("| `-v` | `--version` | Show version information |\n\n");
    
    printf("## Examples\n\n");
    printf("### Arabic numerals:\n");
    printf("```bash\n");
    printf("%s\n", PROGRAM_NAME);
    printf("Think of a number between 1 and 100.\n");
    printf("Is your number greater than 50? (y/n): n\n");
    printf("...\n");
    printf("Your number is 37!\n");
    printf("```\n\n");
    
    printf("### Roman numerals:\n");
    printf("```bash\n");
    printf("%s -r\n", PROGRAM_NAME);
    printf("Think of a number between I and C.\n");
    printf("Is your number greater than L? (y/n): n\n");
    printf("...\n");
    printf("Your number is XXXVII!\n");
    printf("```\n\n");
    
    printf("## Implementation Details\n\n");
    printf("The program uses a lookup table for Roman numeral conversions:\n\n");
    printf("- Range: 1 to 100 (I to C)\n");
    printf("- Case-insensitive Roman numeral parsing\n");
    printf("- Efficient binary search with O(log n) complexity\n\n");
    
    printf("## Doxygen Documentation\n\n");
    printf("The source code is fully documented with Doxygen-style comments.\n");
    printf("Key documented elements include:\n\n");
    printf("- `int_to_roman()`: Convert integer to Roman numeral\n");
    printf("- `roman_to_int()`: Convert Roman numeral to integer\n");
    printf("- `play_game()`: Main game logic\n");
    printf("- `ROMAN_NUMERALS[]`: Lookup table for conversions\n");
    printf("- All major macros and constants\n\n");
    
    printf("## See Also\n\n");
    printf("- man 1 guess - Manual page\n");
    printf("- Doxygen-generated documentation\n");
}

/**
 * @brief Print version information
 */
void print_version(void) {
    printf("%s version %s\n", PROGRAM_NAME, VERSION);
    printf("Number guessing game with Roman numeral support\n");
}

/**
 * @brief Main game function
 * 
 * @param use_roman Boolean flag indicating if Roman numerals should be used
 * @return int Exit status (0 for success)
 * 
 * Implements binary search algorithm to guess user's number. Supports
 * both Arabic and Roman numeral display formats based on use_roman flag.
 */
int play_game(int use_roman) {
    int low = MIN_NUMBER;
    int high = MAX_NUMBER;
    int mid;
    char response[10];
    
    if (use_roman) {
        printf(_("Think of a number between I and C.\n"));
    } else {
        printf(_("Think of a number between 1 and 100.\n"));
    }
    
    while (low < high) {
        mid = low + (high - low) / 2;
        
        if (use_roman) {
            const char *roman = int_to_roman(mid);
            printf(_("Is your number greater than %s? (y/n): "), roman);
        } else {
            printf(_("Is your number greater than %d? (y/n): "), mid);
        }
        
        while (1) {
            if (fgets(response, sizeof(response), stdin) == NULL) {
                fprintf(stderr, _("Error: Unexpected end of input (^D).\n"));
                return 1;
            }
            
            // Remove newline if present
            size_t len = strlen(response);
            if (len > 0 && response[len-1] == '\n') {
                response[len-1] = '\0';
            }
            
            // Check response
            if (strcmp(response, _("y")) == 0) {
                low = mid + 1;  // Search upper half
                break;
            } else if (strcmp(response, _("n")) == 0) {
                high = mid;     // Search lower half
                break;
            } else {
                fprintf(stderr, _("Error: Invalid response. Please enter 'y' or 'n'.\n"));
            }
        }
    }
    
    // We've found the number
    if (use_roman) {
        const char *roman = int_to_roman(low);
        printf(_("Your number is %s!\n"), roman);
    } else {
        printf(_("Your number is %d!\n"), low);
    }
    
    return 0;
}

/**
 * @brief Program entry point
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit status
 * 
 * Parses command line arguments and starts the game with appropriate numeral system.
 */
int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALEDIR);
    textdomain("guess");
    
    int use_roman = 0;
    int show_help = 0;
    int show_help_md = 0;
    int show_version = 0;
    
    // Parse command line options
    struct option long_options[] = {
        {"roman", no_argument, 0, 'r'},
        {"help", no_argument, 0, 'h'},
        {"help-md", no_argument, 0, 0},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int c;
    int option_index = 0;
    while ((c = getopt_long(argc, argv, "rhv", long_options, &option_index)) != -1) {
        switch (c) {
            case 'r':
                use_roman = 1;
                break;
            case 'h':
                show_help = 1;
                break;
            case 0:
                if (strcmp(long_options[option_index].name, "help-md") == 0) {
                    show_help_md = 1;
                }
                break;
            case 'v':
                show_version = 1;
                break;
            case '?':
                return 1;
            default:
                abort();
        }
    }
    
    // Handle help and version options
    if (show_help_md) {
        print_help_md();
        return 0;
    }
    
    if (show_help) {
        print_help(argv[0], getenv("LANG") ? (strstr(getenv("LANG"), "ru") ? "ru" : "en") : "en");
        return 0;
    }
    
    if (show_version) {
        print_version();
        return 0;
    }
    
    // Check for extra arguments
    if (optind < argc) {
        fprintf(stderr, _("Error: Unexpected arguments.\n"));
        fprintf(stderr, _("Try '%s --help' for more information.\n"), argv[0]);
        return 1;
    }
    
    // Start the game
    return play_game(use_roman);
}
