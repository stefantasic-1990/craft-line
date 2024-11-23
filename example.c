#include "craftLine.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char* line;
    do {
        line = craftLine("craftLine->> ");
        write(STDOUT_FILENO, line, sizeof(line));
        write(STDOUT_FILENO, "\x0a", sizeof("\x0a"));
        free(line);
    } while (true);
}