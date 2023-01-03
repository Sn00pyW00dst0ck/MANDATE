#pragma once

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <vector>
#include <string>

#define CTRL_KEY(k) ((k) & 0x1f)

struct Line  {
    size_t start = 0;
    size_t end = 0;
};

class Editor {
public:
    std::string data = "";         // Holds actual characters
    std::vector<Line> lines;        // Holds start and end of each line
    size_t cursor = 0;                  // Holds cursor position in data array

    void cursor_left();     // Cursor Movement Function
    void cursor_right();    // Cursor Movement Function    
    void cursor_up();       // Cursor Movement Function
    void cursor_down();     // Cursor Movement Function

    size_t get_cursor_row();    // Cursor Position Getters
    size_t get_cursor_col();    // Cursor Position Getters

    void do_backspace();    // Buffer Editing Operations
    void do_delete();       // Buffer Editing Operations
    void do_insert(char c); // Buffer Editing Operations
    void recompute_lines(); // Buffer Editing Operations

    void save_to_file(const char *file_path);       // File Operations
    void load_from_file(const char *file_path);     // File Operations
    void get_file_size(const char *file_path);      // File Operations

    void display_lines();

/*
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
*/

};
