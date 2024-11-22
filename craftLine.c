#include "craftLine.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>

char* craftLine(char* prompt) {
    int lineLength = 0;
    int lineCursorPosition = 0;
    int lineDisplayOffset = 0;
    int lineDisplayLength = 0;
    int promptLength = strlen(prompt);

    char * lineBuffer;
    int lineBufferSize = 100;
    lineBuffer = calloc(lineBufferSize, sizeof(char));
    lineBuffer[0] = '\0';

    struct winsize ws;
    int terminalWindowWidth;
    if (ioctl(1, TIOCGWINSZ, &ws) == -1) {terminalWindowWidth = 80;} else {terminalWindowWidth = ws.ws_col - 1;}

    int historyCursorPosition;

    do {
        write(STDOUT_FILENO, "\x1b[0G", strlen("\x1b[0G"));
        write(STDOUT_FILENO, prompt, promptLength);
        write(STDOUT_FILENO, (lineBuffer + lineDisplayOffset), (lineBufferSize < lineDisplayLength) ? lineBufferSize : lineDisplayLength);
        write(STDOUT_FILENO, "\x1b[0K", strlen("\x1b[0K"));

        char cursorEscCode[10];
        snprintf(cursorEscCode, sizeof(cursorEscCode), "\x1b[%iG", promptLength + 1 + lineCursorPosition - lineDisplayOffset);
        write(STDOUT_FILENO, lineCursorPosition, strlen(lineCursorPosition));

        char c;
        read(STDIN_FILENO, &c, 1);

        switch(c) {
            case 13: // enter
                goto returnLine;
            case 8: // ctrl+h
            case 127: // backspace
                if (lineCursorPosition > 0) {
                    memmove(lineBuffer+(lineCursorPosition-1), lineBuffer+lineCursorPosition , lineLength - lineCursorPosition);
                    lineCursorPosition--;
                    lineLength--;
                    lineBuffer[lineLength] = '\0';
                }
                break;
            case 3: // ctrl+c
                exit(EXIT_SUCCESS);
            case 4: // ctrl+d
                break;
            case 20: // ctrl+t
                break; 
            case 16: // ctrl+x
                break; 
            case 14: // ctrl+n
                break;
            case 11: // ctrl+k
                break;
            case 1: // ctrl+a
                break;
            case 5: // ctrl+e
                break;
            case 12: // ctrl+l
                break;
            case 23: // ctrl+w
                break;
            case 21: // ctrl+u - clear input
                free(lineBuffer);
                lineBuffer = calloc(lineBufferSize, sizeof(char));
                lineCursorPosition = 0;
                lineDisplayOffset = 0;
                lineLength = 0;
                break;
            case 27: // escape character
                char escapeSequence[3];
                // read-in the next two characters
                if (read(STDOUT_FILENO, escapeSequence, 1) == -1) {break;}
                if (read(STDOUT_FILENO, escapeSequence+1, 1) == -1) {break;}
                if (escapeSequence[0] == '[') {
                    switch(escapeSequence[1]) {
                        case 'C': // right arrow key
                            if (lineCursorPosition < lineLength) {lineCursorPosition++;}
                            if ((lineCursorPosition - lineDisplayOffset) > lineDisplayLength) {lineDisplayOffset++;}
                            break;
                        case 'D': // left arrow key
                            if (lineCursorPosition > 0) {lineCursorPosition--;}
                            if ((lineCursorPosition - lineDisplayOffset) < 0) {lineDisplayOffset--;}
                            break;
                    }
                }
                break;
            default: // store character in buff
                memmove(lineBuffer+lineCursorPosition+1, lineBuffer+lineCursorPosition, lineLength - lineCursorPosition);
                lineBuffer[lineCursorPosition] = c;
                lineCursorPosition++;
                lineLength++;
                lineBuffer[lineLength] = '\0';
                if ((lineCursorPosition - lineDisplayOffset) > lineDisplayLength) {lineDisplayOffset++;}
                break;
        }
        // allocate more space for buff if required
        if (lineLength + 1 >= lineBufferSize) {
            lineBufferSize += lineBufferSize;
            lineBuffer = realloc(lineBuffer, lineBufferSize);
            if (!lineBuffer) {return NULL;}
        }
    } while (true);

    returnLine:
        write(STDOUT_FILENO, "\r\n", sizeof("\r\n"));
        return lineBuffer;
}