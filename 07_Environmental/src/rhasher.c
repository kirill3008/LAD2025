#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <rhash.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#else
#include <stdio.h>
#endif

#define MAX_HASH_SIZE 128
#define MAX_COMMAND_LENGTH 1024

typedef enum {
    HASH_MD5,
    HASH_SHA1,
    HASH_TTH,
    HASH_UNKNOWN
} hash_algorithm_t;

// Parse algorithm name
hash_algorithm_t parse_algorithm(const char* algo_name) {
    if (strcasecmp(algo_name, "md5") == 0 || strcasecmp(algo_name, "MD5") == 0) {
        return HASH_MD5;
    } else if (strcasecmp(algo_name, "sha1") == 0 || strcasecmp(algo_name, "SHA1") == 0) {
        return HASH_SHA1;
    } else if (strcasecmp(algo_name, "tth") == 0 || strcasecmp(algo_name, "TTH") == 0) {
        return HASH_TTH;
    }
    return HASH_UNKNOWN;
}

int get_rhash_id(hash_algorithm_t algo) {
    switch(algo) {
        case HASH_MD5: return RHASH_MD5;
        case HASH_SHA1: return RHASH_SHA1;
        case HASH_TTH: return RHASH_TTH;
        default: return 0;
    }
}

int hash_string(const char* str, hash_algorithm_t algo, int uppercase, char* output, size_t output_size) {
    int rhash_id = get_rhash_id(algo);
    if (rhash_id == 0) return -1;
    
    unsigned char digest[64];
    int digest_length = rhash_get_digest_size(rhash_id);
    
    if (rhash_msg(rhash_id, str, strlen(str), digest) < 0) {
        return -1;
    }
    
    if (uppercase) {
        rhash_print_bytes(output, digest, digest_length, RHPR_HEX);
        return 0;
    } else {
        rhash_print_bytes(output, digest, digest_length, RHPR_BASE64);
        return 0;
    }
}

int hash_file(const char* filename, hash_algorithm_t algo, int uppercase, char* output, size_t output_size) {
    int rhash_id = get_rhash_id(algo);
    if (rhash_id == 0) return -1;
    
    unsigned char digest[64];
    
    if (rhash_file(rhash_id, filename, digest) < 0) {
        return -1;
    }
    
    int digest_length = rhash_get_digest_size(rhash_id);
    
    if (uppercase) {
        rhash_print_bytes(output, digest, digest_length, RHPR_HEX);
        return 0;
    } else {
        rhash_print_bytes(output, digest, digest_length, RHPR_BASE64);
        return 0;
    }
}

int process_command(const char* command, int show_prompt) {
    char cmd_copy[MAX_COMMAND_LENGTH];
    if (!command) {
        return -1;
    }
    
    size_t input_len = strlen(command);
    if (input_len >= sizeof(cmd_copy)) {
        if (show_prompt) {
            fprintf(stderr, "Error: Command too long\n");
        }
        return -1;
    }
    
    strncpy(cmd_copy, command, sizeof(cmd_copy) - 1);
    cmd_copy[sizeof(cmd_copy) - 1] = '\0';
    
    size_t len = strlen(cmd_copy);
    if (len > 0 && cmd_copy[len-1] == '\n') {
        cmd_copy[len-1] = '\0';
    }
    if (len > 1 && cmd_copy[len-2] == '\r') {
        cmd_copy[len-2] = '\0';
    }
    
    char* algo_name = strtok(cmd_copy, " \t\n");
    if (!algo_name) {
        return 0; // Empty line
    }
    
    char* target = strtok(NULL, " \t\n");
    if (!target) {
        fprintf(stderr, "Error: No target specified\n");
        return -1;
    }
    
    hash_algorithm_t algo = parse_algorithm(algo_name);
    if (algo == HASH_UNKNOWN) {
        fprintf(stderr, "Error: Unknown algorithm '%s'\n", algo_name);
        return -1;
    }
    
    int uppercase_output = isupper((unsigned char)algo_name[0]);
    
    char result[MAX_HASH_SIZE];
    int ret;
    
    if (target[0] == '\"') {
        char* str_start = target + 1;
        char* str_end = strchr(str_start, '\"');
        if (!str_end) {
            fprintf(stderr, "Error: Unclosed quote in string\n");
            return -1;
        }
        *str_end = '\0';
        
        ret = hash_string(str_start, algo, uppercase_output, result, sizeof(result));
    } else {
        ret = hash_file(target, algo, uppercase_output, result, sizeof(result));
    }
    
    if (ret == 0) {
        printf("%s\n", result);
    } else {
        fprintf(stderr, "Error: Failed to compute hash\n");
        return -1;
    }
    
    return 0;
}

void show_help() {
    printf("Usage: rhasher [ALGORITHM TARGET]\n");
    printf("ALGORITHM: md5, sha1, tth (lowercase for Base64, uppercase for hex)\n");
    printf("TARGET: filename or \"quoted string\"\n");
    printf("If no arguments, starts in interactive mode.\n");
}

int main(int argc, char* argv[]) {
    rhash_library_init();
    
    if (argc > 1) {
        if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
            show_help();
            return 0;
        }
        
        char* algo_name = argv[1];
        
        hash_algorithm_t algo = parse_algorithm(algo_name);
        if (algo == HASH_UNKNOWN) {
            fprintf(stderr, "Error: Unknown algorithm '%s'\n", algo_name);
            return 1;
        }
        
        int uppercase_output = isupper((unsigned char)algo_name[0]);
        
        char* target = NULL;
        char reconstructed[MAX_COMMAND_LENGTH];
        
        if (argc == 3) {
            target = argv[2];
        } else if (argc > 3) {
            size_t pos = 0;
            reconstructed[0] = '\0';
            for (int i = 2; i < argc; i++) {
                size_t arg_len = strlen(argv[i]);
                if (pos + arg_len >= sizeof(reconstructed) - 1) {
                    fprintf(stderr, "Error: Command too long\n");
                    return 1;
                }
                strcpy(reconstructed + pos, argv[i]);
                pos += arg_len;
                
                if (i < argc - 1) {
                    if (pos + 1 >= sizeof(reconstructed) - 1) {
                        fprintf(stderr, "Error: Command too long\n");
                        return 1;
                    }
                    strcpy(reconstructed + pos, " ");
                    pos += 1;
                }
            }
            target = reconstructed;
        } else {
            fprintf(stderr, "Error: No target specified\n");
            return 1;
        }
        
        char result[MAX_HASH_SIZE];
        int ret;
        
        FILE* test_file = fopen(target, "r");
        if (test_file) {
            fclose(test_file);
            ret = hash_file(target, algo, uppercase_output, result, sizeof(result));
        } else {
            ret = hash_string(target, algo, uppercase_output, result, sizeof(result));
        }
        
        if (ret == 0) {
            printf("%s\n", result);
            return 0;
        } else {
            fprintf(stderr, "Error: Failed to compute hash\n");
            return 1;
        }
    }
    
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    
    #ifdef USE_READLINE
    int use_readline = isatty(fileno(stdin));
    #endif
    
    printf("rhasher - Hash calculator (Ctrl+D to exit)\n");
    
    while (1) {
        #ifdef USE_READLINE
        if (use_readline) {
            line = readline("rhasher> ");
            if (!line) break; // EOF
            if (strlen(line) > 0) {
                add_history(line);
            }
            process_command(line, 1);
            free(line);
            line = NULL;
        } else {
            printf("rhasher> ");
            fflush(stdout);
            
            line = NULL;
            len = 0;
            
            read = getline(&line, &len, stdin);
            if (read == -1) {
                if (line) {
                    free(line);
                    line = NULL;
                }
                break;
            }
            if (read > 0) {
                process_command(line, 1);
            }
            if (line) {
                free(line);
                line = NULL;
            }
        }
        #else
        printf("rhasher> ");
        fflush(stdout);
        
        read = getline(&line, &len, stdin);
        if (read == -1) {
            if (line) {
                free(line);
                line = NULL;
            }
            break;
        }
        if (read > 0) {
            process_command(line, 1);
        }
        if (line) {
            free(line);
            line = NULL;
        }
        #endif
    }
    
    return 0;
}
