#include <unistd.h>
#include <termios.h>

struct termios initial_terminal_settings;

int term_enable_raw()
{
    struct termios modified_terminal_settings;
    if (isatty(STDIN_FILENO)) {
        // save current settings for later restore.
        tcgetattr(STDIN_FILENO, &initial_terminal_settings);

        modified_terminal_settings = initial_terminal_settings;

        // configure terminal input flags; turn off special handling of input.
        // no break-to-signal, CR→NL mapping, parity check, stripping, or XON/XOFF flow control.
        modified_terminal_settings.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

        // configure output flags; no post-processing, send output bytes as-is, no NL→CRNL conversion, etc.
        modified_terminal_settings.c_oflag &= ~(OPOST);

        // configure control flags; use 8-bit characters.
        modified_terminal_settings.c_cflag |= CS8;

        // configure local flags; turn on raw input mode.
        // no echo, no canonical line buffering/editing, no extended functions, no signal generation.
        modified_terminal_settings.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

        // configure read buffer settings: return after 1 byte, no timeout
        modified_terminal_settings.c_cc[VMIN]  = 1;
        modified_terminal_settings.c_cc[VTIME] = 0;

        // apply modified settings and flush pending input.
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &modified_terminal_settings);
    } else {
        return -1;
    }
    return 0;
}

int term_disable_raw()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &initial_terminal_settings);
}