#include "editor.h"

char *craftline(char *prompt) {
    char *line = line_edit(prompt);
    return line;
}