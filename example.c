#include "craftLine.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    char* line;
    do {
        line = craftLine("craftLine->> ");
        write(STDOUT_FILENO, line, strlen(line));
        write(STDOUT_FILENO, "\x0a", strlen("\x0a"));
        free(line);
    } while (true);
}