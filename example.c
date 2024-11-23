#include "craftLine.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    char* line;

    do {
        line = craftLine("craftLine->> ");
    } while (true);
}