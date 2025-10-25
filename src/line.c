#include "line.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

void redraw_line(char *prompt, int prompt_len, line_state *line)
{
    // repaint: move to col 0, print prompt + visible slice of line buffer, clear to end of window (removes leftover chars if needed).
    write(STDOUT_FILENO, "\x1b[0G", 4);
    write(STDOUT_FILENO, prompt, prompt_len);
    write(STDOUT_FILENO, line->line_buffer + line->line_display_offset, line->curr_line_len < line->line_display_len ? line->curr_line_len : line->line_display_len);
    write(STDOUT_FILENO, "\x1b[0K", 4);

    // place cursor after prompt at logical position.
    char cursor_esc_code[10];
    snprintf(cursor_esc_code, sizeof(cursor_esc_code), "\x1b[%iG", prompt_len + 1 + line->curr_cursor_pos - line->line_display_offset);
    write(STDOUT_FILENO, cursor_esc_code, strlen(cursor_esc_code));
}

void init_line(int prompt_len, line_state *line)
{
    // Initialize line state
    memset(line, 0, sizeof(*line));
    line->line_buffer_size = 100;
    line->line_buffer = malloc(line->line_buffer_size);
    line->line_buffer[0] = '\0';

    // determine terminal window width to compute visible line region.
    struct winsize ws;
    int term_win_width;

    if (ioctl(1, TIOCGWINSZ, &ws) == -1) {
        term_win_width = 80;
    } else {
        term_win_width = ws.ws_col - 1;
    }

    line->line_display_len = term_win_width - prompt_len;
}