#include <unistd.h>
#include <termios.h>

static struct termios prev_term_conf;

int term_enable_raw(void)
{
    struct termios raw_term_conf;

    if (isatty(STDIN_FILENO) == 0)
        return -1;

    if (tcgetattr(STDIN_FILENO, &prev_term_conf) == -1)
        return -1;
    
    raw_term_conf = prev_term_conf;

    // Configure terminal input flags.
    // No break-to-signal, CR→NL mapping, parity check, stripping, or XON/XOFF flow control.
    raw_term_conf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Configure output flags.
    // No post-processing: send output bytes as-is, no NL→CRNL conversion, etc.
    raw_term_conf.c_oflag &= ~(OPOST);

    // Configure control flags: use 8-bit characters.
    raw_term_conf.c_cflag |= CS8;

    // Configure local flags: turn on raw input mode.
    // No echo, no canonical line buffering/editing, no extended functions, no signal generation.
    raw_term_conf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    // Configure read buffer settings: return after 1 byte, no timeout
    raw_term_conf.c_cc[VMIN]  = 1;
    raw_term_conf.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_term_conf) == -1) return -1;

    return 0;
}

int term_disable_raw(void)
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &prev_term_conf) == -1) return -1;
    
    return 0;
}