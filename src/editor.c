#include "termctrl.h"
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void redraw_line(char *line_buffer, char *prompt, int prompt_len, int curr_line_len, int line_display_len, int curr_cursor_pos, int line_display_offset)
{
    // repaint: move to col 0, print prompt + visible slice of line buffer, clear to end of window (removes leftover chars if needed).
    write(STDOUT_FILENO, "\x1b[0G", 4);
    write(STDOUT_FILENO, prompt, prompt_len);
    write(STDOUT_FILENO, line_buffer + line_display_offset, curr_line_len < line_display_len ? curr_line_len : line_display_len);
    write(STDOUT_FILENO, "\x1b[0K", 4);

    // place cursor after prompt at logical position.
    char cursor_esc_code[10];
    snprintf(cursor_esc_code, sizeof(cursor_esc_code), "\x1b[%iG", prompt_len + 1 + curr_cursor_pos - line_display_offset);
    write(STDOUT_FILENO, cursor_esc_code, strlen(cursor_esc_code));
}

char *line_edit(char *prompt)
{
    int prompt_len = strlen(prompt);

    // Initialize line buffer
    char *line_buffer;
    int line_buffer_size = 100;
    line_buffer = malloc(line_buffer_size);
    line_buffer[0] = '\0';

    // logical line state.
    int curr_line_len = 0;        // current line length
    int curr_cursor_pos = 0;      // current cursor position within line buffer.
    int line_display_offset = 0;  // horizontal scroll offset; line is printed on screen starting here.
    int line_display_len = 0;     // visible chars available on screen after prompt is printed; depends on window size.

    // determine terminal window width to compute visible line region.
    struct winsize ws;
    int term_win_width;

    if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
        term_win_width = 80;
    } else {
        term_win_width = ws.ws_col - 1;
    }

    line_display_len = term_win_width - prompt_len;

    if (term_enable_raw() == -1) return NULL;

    do {
        redraw_line(line_buffer, prompt, prompt_len, curr_line_len, line_display_len, curr_cursor_pos, line_display_offset);

        // read a single byte in raw mode.
        char c;
        if (read(STDIN_FILENO, &c, 1) == -1) { 
            term_disable_raw();
            return NULL; 
        }

        // handle input
        switch(c) {
            case 13: // enter
                goto return_line;

            case 8:  // ctrl+h
            case 127: // Backspace
                if (curr_cursor_pos > 0) {
                    // delete chararacter before cursor.
                    memmove(line_buffer + (curr_cursor_pos - 1), line_buffer + curr_cursor_pos, curr_line_len - curr_cursor_pos);
                    curr_cursor_pos--;
                    curr_line_len--;
                    line_buffer[curr_line_len] = '\0';
                    if (line_display_offset > 0)
                        line_display_offset--;
                }
                break;

            case 3: // ctrl+c: restore TTY to original settings then exit.
                term_disable_raw();
                write(STDOUT_FILENO, "\x0a", 1);
                exit(EXIT_SUCCESS);

            case 4: // ctrl+d:
                if (curr_line_len == 0) {
                    term_disable_raw();
                    write(STDOUT_FILENO, "\n", 1);
                    free(line_buffer);
                    return NULL;
                }
                break;

            case 20: // ctrl+t:
                break;

            case 16: // ctrl+x:
                break;

            case 14: // ctrl+n:
                break;

            case 11: // ctrl+k:
                break;

            case 1:  // ctrl+a:
                break;

            case 5:  // ctrl+e:
                break;

            case 12: // ctrl+l:
                break;

            case 23: // ctrl+w:
                break;

            case 21: // ctrl+u: clear entire input.
                free(line_buffer);
                line_buffer = calloc(line_buffer_size, sizeof(char));
                curr_cursor_pos = 0;
                line_display_offset = 0;
                curr_line_len = 0;
                break;

            case 27: { // esc: parse simple CSI arrow keys.
                char esc_sequence[3];
                // read next two bytes; ignore errors.
                if (read(STDIN_FILENO, esc_sequence, 1) == -1) 
                    break;
                if (read(STDIN_FILENO, esc_sequence+1, 1) == -1) 
                    break;
                if (esc_sequence[0] == '[') {
                    switch(esc_sequence[1]) {
                        case 'C': // right arrow: move cursor to right
                            if (curr_cursor_pos < curr_line_len) 
                                curr_cursor_pos++;
                            if (curr_cursor_pos - line_display_offset > line_display_len) 
                                line_display_offset++;
                            break;
                        case 'D': // left arrow: move cursor to left
                            if (curr_cursor_pos > 0) 
                                curr_cursor_pos--;
                            if (curr_cursor_pos < line_display_offset)
                                line_display_offset--;
                            break;
                        case 'A': // up arrow: move to previous history entry.
                            break;
                        case 'B': // down arrow: move to next history entry, or currently buffered line.
                            break;
                    }
                }
                break;
            }

            default: // insert character at cursor.
                memmove(line_buffer + curr_cursor_pos + 1, line_buffer + curr_cursor_pos, curr_line_len - curr_cursor_pos);
                line_buffer[curr_cursor_pos] = c;
                curr_cursor_pos++;
                curr_line_len++;
                line_buffer[curr_line_len] = '\0';
                if (curr_cursor_pos - line_display_offset > line_display_len)
                    line_display_offset++;
                break;
        }

        // grow buffer if needed.
        if (curr_line_len + 1 >= line_buffer_size) {
            line_buffer_size += line_buffer_size;
            line_buffer = realloc(line_buffer, line_buffer_size);
            if (!line_buffer) return NULL;
        }
    } while (1);

    return_line:
        term_disable_raw();
        write(STDOUT_FILENO, "\x0a", 1);

        return line_buffer;
}