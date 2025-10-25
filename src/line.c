#include "line.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void redraw_line(char *prompt, int prompt_len, line_state line)
{
    // repaint: move to col 0, print prompt + visible slice of line buffer, clear to end of window (removes leftover chars if needed).
    write(STDOUT_FILENO, "\x1b[0G", 4);
    write(STDOUT_FILENO, prompt, prompt_len);
    write(STDOUT_FILENO, line.line_buffer + line.line_display_offset, line.curr_line_len < line.line_display_len ? line.curr_line_len : line.line_display_len);
    write(STDOUT_FILENO, "\x1b[0K", 4);

    // place cursor after prompt at logical position.
    char cursor_esc_code[10];
    snprintf(cursor_esc_code, sizeof(cursor_esc_code), "\x1b[%iG", prompt_len + 1 + line.curr_cursor_pos - line.line_display_offset);
    write(STDOUT_FILENO, cursor_esc_code, strlen(cursor_esc_code));
}