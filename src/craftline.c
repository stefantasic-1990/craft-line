#include "termctrl.h"
#include "line.h"
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

static int read_escseq(char escseq[2]) {
    for (int i = 0; i < 2; i++) {
        ssize_t n;
        do {
            n = read(STDIN_FILENO, escseq + i, 1);
        } while (n == -1 && errno == EINTR);  // retry if interrupted
        if (n == -1) return -1;
    }
    return 0;
}

char *craftline(char *prompt)
{
    int prompt_len = strlen(prompt);

    // Initialize line state
    line_state line = {0};
    line.line_buffer_size = 100;
    line.line_buffer = malloc(line.line_buffer_size);
    line.line_buffer[0] = '\0';

    // determine terminal window width to compute visible line region.
    struct winsize ws;
    int term_win_width;

    if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
        term_win_width = 80;
    } else {
        term_win_width = ws.ws_col - 1;
    }

    line.line_display_len = term_win_width - prompt_len;

    if (term_enable_raw() == -1) return NULL;

    do {
        redraw_line(prompt, prompt_len, line);

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
                if (line.curr_cursor_pos > 0) {
                    // delete chararacter before cursor.
                    memmove(line.line_buffer + (line.curr_cursor_pos - 1), line.line_buffer + line.curr_cursor_pos, line.curr_line_len - line.curr_cursor_pos);
                    line.curr_cursor_pos--;
                    line.curr_line_len--;
                    line.line_buffer[line.curr_line_len] = '\0';
                    if (line.line_display_offset > 0)
                        line.line_display_offset--;
                }
                break;

            case 3: // ctrl+c: restore TTY to original settings then exit.
                term_disable_raw();
                write(STDOUT_FILENO, "\x0a", 1);
                exit(EXIT_SUCCESS);

            case 4: // ctrl+d:
                if (line.curr_line_len == 0) {
                    term_disable_raw();
                    write(STDOUT_FILENO, "\n", 1);
                    free(line.line_buffer);
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
                free(line.line_buffer);
                line.line_buffer = calloc(line.line_buffer_size, sizeof(char));
                line.curr_cursor_pos = 0;
                line.line_display_offset = 0;
                line.curr_line_len = 0;
                break;

            case 27: // esc: parse simple CSI arrow keys.
                char escseq[2];
                if (read_escseq(escseq) == 0 && escseq[0] == '[') {
                    switch(escseq[1]) {
                        case 'C': // right arrow: move cursor to right
                            if (line.curr_cursor_pos < line.curr_line_len) 
                                line.curr_cursor_pos++;
                            if (line.curr_cursor_pos - line.line_display_offset > line.line_display_len) 
                                line.line_display_offset++;
                            break;
                        case 'D': // left arrow: move cursor to left
                            if (line.curr_cursor_pos > 0) 
                                line.curr_cursor_pos--;
                            if (line.curr_cursor_pos < line.line_display_offset)
                                line.line_display_offset--;
                            break;
                        case 'A': // up arrow: move to previous history entry.
                            break;
                        case 'B': // down arrow: move to next history entry, or currently buffered line.
                            break;
                    }
                }
                break;

            default: // insert character at cursor.
                memmove(line.line_buffer + line.curr_cursor_pos + 1, line.line_buffer + line.curr_cursor_pos, line.curr_line_len - line.curr_cursor_pos);
                line.line_buffer[line.curr_cursor_pos] = c;
                line.curr_cursor_pos++;
                line.curr_line_len++;
                line.line_buffer[line.curr_line_len] = '\0';
                if (line.curr_cursor_pos - line.line_display_offset > line.line_display_len)
                    line.line_display_offset++;
                break;
        }

        // grow buffer if needed.
        if (line.curr_line_len + 1 >= line.line_buffer_size) {
            line.line_buffer_size += line.line_buffer_size;
            line.line_buffer = realloc(line.line_buffer, line.line_buffer_size);
            if (!line.line_buffer) return NULL;
        }
    } while (1);

    return_line:
        term_disable_raw();
        write(STDOUT_FILENO, "\x0a", 1);

        return line.line_buffer;
}