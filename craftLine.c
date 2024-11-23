#include "craftLine.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <termios.h>
#include <sys/ioctl.h>

struct termios initial_terminal_settings;

int enableRawTerminal(bool state) {
    static struct termios initial_terminal_settings;
    static bool inRawMode = false;
    static bool initialized = false;

    if (state == true && inRawMode == false) {

        if (isatty(STDIN_FILENO)) { 
            tcgetattr(STDIN_FILENO, &initial_terminal_settings);
            struct termios modified_terminal_settings;
            modified_terminal_settings = initial_terminal_settings;

            modified_terminal_settings.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
            modified_terminal_settings.c_oflag &= ~(OPOST);
            modified_terminal_settings.c_cflag |= (CS8);
            modified_terminal_settings.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
            modified_terminal_settings.c_cc[VMIN] = 1; 
            modified_terminal_settings.c_cc[VTIME] = 0;
            
            tcsetattr(STDIN_FILENO,TCSAFLUSH,&modified_terminal_settings);
            inRawMode = true;
        } else {return -1;} 

    } else if (state == false && inRawMode == true) {
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&initial_terminal_settings);
        inRawMode = false;
    }

    return 0;
}

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
    lineDisplayLength = terminalWindowWidth - promptLength;

    int historyCursorPosition;

    enableRawTerminal(true);

    do {
        write(STDOUT_FILENO, "\x1b[0G", strlen("\x1b[0G"));
        write(STDOUT_FILENO, prompt, promptLength);
        write(STDOUT_FILENO, (lineBuffer + lineDisplayOffset), (lineBufferSize < lineDisplayLength) ? lineBufferSize : lineDisplayLength);
        write(STDOUT_FILENO, "\x1b[0K", strlen("\x1b[0K"));

        char cursorEscCode[10];
        snprintf(cursorEscCode, sizeof(cursorEscCode), "\x1b[%iG", promptLength + 1 + lineCursorPosition - lineDisplayOffset);
        write(STDOUT_FILENO, cursorEscCode, strlen(cursorEscCode));

        char c;
        read(STDOUT_FILENO, &c, 1);

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
                enableRawTerminal(false);
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
            case 27: { // escape character
                char escapeSequence[3];
                // read-in the next two characters
                if (read(STDIN_FILENO, escapeSequence, 1) == -1) {break;}
                if (read(STDIN_FILENO, escapeSequence+1, 1) == -1) {break;}
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
            }
            default: // store character in buff
                memmove(lineBuffer+lineCursorPosition+1, lineBuffer+lineCursorPosition, lineLength-lineCursorPosition);
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
        enableRawTerminal(false);
        write(STDOUT_FILENO, "\x0a", sizeof("\x0a"));
        return lineBuffer;
}