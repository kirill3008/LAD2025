#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libintl.h>
#include <locale.h>

#define _(STRING) gettext(STRING)

int main() {
    setlocale(LC_ALL, "");
    bindtextdomain("guess", LOCALEDIR);
    textdomain("guess");
    
    int low = 1;
    int high = 100;
    int mid;
    char response[10];
    
    printf(_("Think of a number between 1 and 100.\n"));
    
    while (low < high) {
        mid = low + (high - low) / 2;
        
        printf(_("Is your number greater than %d? (y/n): "), mid);
        
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
    printf(_("Your number is %d!\n"), low);
    
    return 0;
}
