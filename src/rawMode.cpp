#include "rawMode.h"

struct termios orig_termios;

void die(const char* s)  {
    // Reset Screen On Death
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    // Write Error and Exit
    perror(s);
    exit(1);
}

void enableRawMode()  {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1; // tenths of second (100ms here)

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode()  {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)  die("tcsetattr");
}
