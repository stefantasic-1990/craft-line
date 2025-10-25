#ifndef LINE_H
#define LINE_H

typedef struct {
    char *line_buffer;
    int line_buffer_size;     // current size of the line buffer
    int curr_line_len;        // current line length
    int curr_cursor_pos;      // current cursor position within line buffer.
    int line_display_offset;  // horizontal scroll offset; line is printed on screen starting here.
    int line_display_len;     // visible chars available on screen after prompt is printed; depends on window size.
} line_state;

void redraw_line(char *prompt, int prompt_len, line_state line);

#endif