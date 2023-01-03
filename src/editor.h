#pragma once

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey  {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DELETE_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

struct writeBuffer  {
    char *b;
    int len;
};
#define WBUF_INIT {NULL, 0}






class Editor  {
    private:
        int cursorX, cursorY;
        int screenRows, screenCols;


        int readKey();
        void moveCursor(int key);

        void drawRows(struct writeBuffer *wb);

    public:
        Editor();
        ~Editor();

        void refreshScreen();
        void processKeyPress();

};
