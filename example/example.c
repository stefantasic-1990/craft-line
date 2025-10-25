#include "craftline.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    char* line;
    do {
        // prompt the user and read a line with craftLine
        line = craftline("craftLine->> ");

        write(STDOUT_FILENO, line, strlen(line)); // echo entered line back to STDOUT
        write(STDOUT_FILENO, "\x0a", strlen("\x0a")); // print a newline

        // free the line buffer allocated and returned by craftLine
        free(line);

        // loop forever until user interrupts (e.g. ctrl+c)
    } while (1);
}