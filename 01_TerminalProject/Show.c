#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <stddef.h>

// Тестовый коментарий на русском!

#ifndef mvwaddwstr
// my ncurses header seem to not have this specific function
// even thought it is present in .so file from same lib(
int mvwaddwstr(WINDOW *win, int y, int x, const wchar_t *wstr);
#endif

#define ESC 27

int load_file(const char *filename, wchar_t ***lines, int *total_lines) {
    FILE *file;
    int line_count = 0;
    char c;
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t read_size;
    int len;
    
    file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 1;
    }
    
    // Count lines in the file
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            line_count++;
        }
    }
    if (c != '\n' && ftell(file) > 0) {
        line_count++;
    }
    rewind(file);
    
    // Allocate memory for the lines
    *lines = (wchar_t **)malloc((line_count + 1) * sizeof(wchar_t *));
    if (*lines == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        fclose(file);
        return 1;
    }
    
    // Read file content
    *total_lines = 0;
    while (*total_lines < line_count && (read_size = getline(&buffer, &buffer_size, file)) != -1) {
        len = (int)read_size;
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }
        
        (*lines)[*total_lines] = (wchar_t *)calloc(len + 1, sizeof(wchar_t));
        if ((*lines)[*total_lines] == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            break;
        }
        
        mbstowcs((*lines)[*total_lines], buffer, len + 1);
        (*total_lines)++;
    }
    
    if (buffer != NULL) {
        free(buffer);
    }
    
    fclose(file);
    return 0;
}


void show_status(WINDOW *win, int top_line, int total_lines, int rows, int left_col) {
    int bottom_row = rows - 1;
    int percent = (total_lines > 0) ? (top_line * 100) / (total_lines - rows + 1) : 0;
    if (percent > 100) percent = 100;

    wmove(win, bottom_row, 0);
    wclrtoeol(win);

    char status[100];
    snprintf(status, sizeof(status), "Line: %d/%d (%d%%) Col: %d", 
             top_line + 1, total_lines, percent, left_col + 1);
    wattron(win, A_REVERSE);
    mvwprintw(win, bottom_row, 0, "%-*s", getmaxx(win) - 1, status);
    wattroff(win, A_REVERSE);
}

void update_display(WINDOW *win, wchar_t **lines, int total_lines, int top_line, int rows, int left_col) {
    werase(win);
    int display_rows = rows - 1;
    int max_width = getmaxx(win);
    wchar_t *display_buffer = NULL;
    
    display_buffer = (wchar_t *)malloc((max_width + 1) * sizeof(wchar_t));
    if (display_buffer == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return;
    }
    
    for (int i = 0; i < display_rows && (top_line + i) < total_lines; i++) {
        wchar_t *line = lines[top_line + i];
        int line_len = wcslen(line);
        
        if (left_col < line_len) {
            wcsncpy(display_buffer, line + left_col, max_width);
            display_buffer[max_width] = L'\0';
            mvwaddwstr(win, i, 0, display_buffer);
        } else {
            mvwaddwstr(win, i, 0, L"");
        }
    }
    
    free(display_buffer);
    show_status(win, top_line, total_lines, rows, left_col);
    wrefresh(win);
}

int main(int argc, char ** argv) {
    setlocale(LC_ALL, "");
    WINDOW *border_win, *content_win;
    int ch, rows, cols, content_rows, content_cols;
    
    wchar_t **lines;
    int total_lines = 0;
    int top_line = 0;
    int left_col = 0;
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    
    if (load_file(argv[1], &lines, &total_lines) != 0) {
        return 1;
    }

     
    initscr();
    start_color();
    use_default_colors();
    
    if (!has_ic()) {
        endwin();
        fprintf(stderr, "Warning: Terminal may not support wide characters\n");
    }
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    
    getmaxyx(stdscr, rows, cols);
    
    border_win = newwin(rows, cols, 0, 0);
    box(border_win, 0, 0);
    
    wchar_t wtitle[256];
    swprintf(wtitle, sizeof(wtitle) / sizeof(wchar_t), L" %s ", argv[1]);
    mvwaddwstr(border_win, 0, (cols - wcslen(wtitle)) / 2, wtitle);
    wrefresh(border_win);

    content_rows = rows - 2;
    content_cols = cols - 2;
    content_win = newwin(content_rows, content_cols, 1, 1);
    if (content_win == NULL) {
        endwin();
        fprintf(stderr, "Error creating content window\n");
        return 1;
    }
    scrollok(content_win, TRUE);
    keypad(content_win, TRUE);
     
    wclear(content_win);
    
    
    update_display(content_win, lines, total_lines, top_line, content_rows, left_col);
    
    while ((ch = wgetch(content_win)) != ESC && ch != 'q') {
        int display_updated = 0;
        switch (ch) {
            case KEY_UP:
                if (top_line > 0) {
                    top_line--;
                    display_updated = 1;
                }
                break;
                
            case KEY_HOME:
                if (top_line != 0) {
                    top_line = 0;
                    left_col = 0;
                    display_updated = 1;
                }
                break;
                
            case KEY_END:
                {
                    int max_top = total_lines - (content_rows - 1);
                    if (max_top < 0) max_top = 0;
                    if (top_line != max_top) {
                        top_line = max_top;
                        display_updated = 1;
                    }
                }
                break;
                
            case KEY_PPAGE:
                if (top_line > 0) {
                    int display_rows = content_rows - 1;
                    top_line = (top_line > display_rows) ? top_line - display_rows : 0;
                    display_updated = 1;
                }
                break;
                
            case KEY_DOWN:
            case ' ':
                if (top_line < total_lines - (content_rows - 1)) {
                    top_line++;
                    display_updated = 1;
                }
                break;
                
            case KEY_NPAGE:
                if (top_line < total_lines - content_rows) {
                    int display_rows = content_rows - 1;
                    int max_top = total_lines - display_rows;
                    if (max_top < 0) max_top = 0;
                    top_line = (top_line + display_rows < max_top) ? top_line + display_rows : max_top;
                    display_updated = 1;
                }
                break;
                  
              case KEY_LEFT:
                  if (left_col > 0) {
                      left_col--;
                      display_updated = 1;
                  }
                  break;
                  
              case KEY_RIGHT:
                  left_col++;
                  display_updated = 1;
                  break;
                  
              case 'h':
                  if (left_col != 0) {
                      left_col = 0;
                      display_updated = 1;
                  }
                  break;
        }
         
         if (display_updated) {
             update_display(content_win, lines, total_lines, top_line, content_rows, left_col);
         }
    }
    
    for (int i = 0; i < total_lines; i++) {
        free(lines[i]);
    }
    free(lines);
    
    delwin(content_win);
    delwin(border_win);
    endwin();
}
