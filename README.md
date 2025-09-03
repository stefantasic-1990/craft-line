# craftLine

CraftLine is a minimal interactive line editor for POSIX terminals.
I wrote it to be used as an input editing component for CLI programs.
Think "bare-bones GNU Readline", and you get the picture.

To use it, save `craftLine.c` and `craftLine.h` and import the header file into your program.
You can then call `char* craftLine(char* prompt)` within your code to prompt the user for input.

In specific, when called it:
1. Flips the terminal into raw mode with `termios`.
2. Provides basic single-line editing capabilities to the user, including a scrollable command history.
3. Returns the finalized line as a heap-allocated string when the user presses `ENTER`.
4. Flips the terminal back to its original settings before exiting.

In the last section of this `README.md` you can find a simple demo.

### Disclaimer

This program is not thoroughly tested and was written by me for educational purposes.
In other words, I wouldn't be surprised if there are sneaky bugs I missed.
On the other hand, I'm sure that there are things that can be improved.

## Features

These are the general features:
- Written in plain C with no external dependencies.
- Raw mode input, byte-by-byte key handling.
- Single-line editing, minimal on-screen re-paint using ANSI escape sequences.
- Simple persistent history stored as `craftLineHistory.txt`.

## Line Editing Key Bindings

These are the currently implemented keys for line editing:
1. `ENTER`: finish editing line and return it.
2. `Left`/`Right`: move cursor left and right.
3. `Up`/`Down`: scroll through the history.
4. `Backspace`: delete char to the left of cursor.
5. `CTRL+U`: clear entire line editing buffer.
6. `CTRL+C`: exit process.

## Demo

The repo contains a file named `example.c` which can be used as a demo of how CraftLine works.
To run it simply compile it together with `craftLine.c` and execute the output program.

For instance, to compile with `gcc`:
<pre>gcc example.c craftLine.c -o example</pre>

Once you run the executable you'll be prompted to edit a line.
When you are done, press `ENTER`, and the input line will be echoed back to the terminal screen.
To exit, just press `CTRL+C`.