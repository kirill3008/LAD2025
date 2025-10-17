#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

#define MAX_MATCHES 10  // Capture groups 0-9 (0 is the whole match)
#define MAX_SUBSTITUTIONS 100
#define MAX_ERROR_MSG 1024
#define MAX_RESULT_SIZE 4096

#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RESET   "\033[0m"

const char *colors[] = {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
};
const int num_colors = sizeof(colors) / sizeof(colors[0]);

// Function to print regex error
void print_regex_error(int error_code, regex_t *regex) {
    char error_message[MAX_ERROR_MSG];
    regerror(error_code, regex, error_message, MAX_ERROR_MSG);
    fprintf(stderr, "Regex error: %s\n", error_message);
}

int process_substitution(const char *substitution, char *result, 
                         regmatch_t *matches, int nmatch, const char *input, 
                         bool use_colors) {
    int result_pos = 0;
    int subst_pos = 0;
    int ref_count = 0;
    
    while (substitution[subst_pos] != '\0' && result_pos < MAX_RESULT_SIZE - 1) {
        if (substitution[subst_pos] == '\\') {
            subst_pos++;
            
            if (substitution[subst_pos] == '\\') {
                result[result_pos++] = '\\';
            } else if (substitution[subst_pos] >= '0' && substitution[subst_pos] <= '9') {
                int group = substitution[subst_pos] - '0';
                ref_count++;
                
                if (ref_count > MAX_SUBSTITUTIONS) {
                    fprintf(stderr, "Error: Too many references in substitution string (max %d)\n", 
                            MAX_SUBSTITUTIONS);
                    return -1;
                }
                
                if (group >= nmatch || matches[group].rm_so == -1) {
                    fprintf(stderr, "Error: Reference to non-existent group \\%d\n", group);
                    return -1;
                }
                
                int match_len = matches[group].rm_eo - matches[group].rm_so;
                if (result_pos + match_len >= MAX_RESULT_SIZE - 1) {
                    fprintf(stderr, "Error: Result string too large\n");
                    return -1;
                }
                
                if (use_colors && group > 0) {
                    const char *color = colors[(group - 1) % num_colors];
                    int color_len = strlen(color);
                    if (result_pos + color_len >= MAX_RESULT_SIZE - 1) {
                        fprintf(stderr, "Error: Result string too large\n");
                        return -1;
                    }
                    strcpy(result + result_pos, color);
                    result_pos += color_len;
                }
                
                strncpy(result + result_pos, 
                        input + matches[group].rm_so, 
                        match_len);
                result_pos += match_len;
                
                if (use_colors && group > 0) {
                    int reset_len = strlen(COLOR_RESET);
                    if (result_pos + reset_len >= MAX_RESULT_SIZE - 1) {
                        fprintf(stderr, "Error: Result string too large\n");
                        return -1;
                    }
                    strcpy(result + result_pos, COLOR_RESET);
                    result_pos += reset_len;
                }
            } else {
                fprintf(stderr, "Warning: Unknown escape sequence '\\%c'\n", substitution[subst_pos]);
                result[result_pos++] = '\\';
                result[result_pos++] = substitution[subst_pos];
            }
        } else {
            result[result_pos++] = substitution[subst_pos];
        }
        
        subst_pos++;
    }
    
    result[result_pos] = '\0';
    return 0;
}

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [-c] regexp substitution string\n", program_name);
    fprintf(stderr, "  -c    Enable colored output for capture groups\n");
}

int main(int argc, char *argv[]) {
    bool use_colors = false;
    int arg_offset = 0;
    
    // Check for color option
    if (argc > 1 && strcmp(argv[1], "-c") == 0) {
        use_colors = true;
        arg_offset = 1;
    }
    
    if (argc != 4 + arg_offset) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *pattern = argv[1 + arg_offset];
    const char *substitution = argv[2 + arg_offset];
    const char *input = argv[3 + arg_offset];
    
    regex_t regex;
    int ret;
    
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) {
        print_regex_error(ret, &regex);
        regfree(&regex);
        return EXIT_FAILURE;
    }
    
    regmatch_t matches[MAX_MATCHES];
    ret = regexec(&regex, input, MAX_MATCHES, matches, 0);
    
    if (ret == REG_NOMATCH) {
        printf("%s\n", input);
        regfree(&regex);
        return EXIT_SUCCESS;
    } else if (ret != 0) {
        print_regex_error(ret, &regex);
        regfree(&regex);
        return EXIT_FAILURE;
    }
    
    char result[MAX_RESULT_SIZE] = {0};
    int result_len = 0;
    
    int prefix_len = matches[0].rm_so;
    if (prefix_len > 0) {
        strncpy(result, input, prefix_len);
        result_len = prefix_len;
    }
    
    char substituted[MAX_RESULT_SIZE] = {0};
    if (process_substitution(substitution, substituted, matches, MAX_MATCHES, input, use_colors) != 0) {
        regfree(&regex);
        return EXIT_FAILURE;
    }
    
    strcpy(result + result_len, substituted);
    result_len += strlen(substituted);
    
    strcpy(result + result_len, input + matches[0].rm_eo);
    
    printf("%s\n", result);
    
    regfree(&regex);
    return EXIT_SUCCESS;
}
