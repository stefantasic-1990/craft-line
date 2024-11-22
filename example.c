#include "craftLine.h"
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    char* line;
    do {
        line = craftLine("craftLine >>");
        write(STDOUT_FILENO, line, sizeof(line));
        write(STDOUT_FILENO, "\r\n", sizeof("\r\n"));
    } while (true);
}