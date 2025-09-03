# craftLine

## What is CraftLine?

CraftLine is a minimal interactive line editor for POSIX terminals.
It's meant to be used as an input editing component for CLI programs.
Think "bare-bones GNU Readline".

More specifically, when called it:
1. Flips the terminal into raw mode with `termios`.
2. Provides basic single-line editing capabilities to the user, including a scrollable command history.
3. Returns the finalized line as a heap-allocated string when the user presses `ENTER`.
4. Flips the terminal back to its original settings before exiting.