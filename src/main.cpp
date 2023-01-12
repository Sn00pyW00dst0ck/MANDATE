/**
 * Work on this in WSL
 * 
 * https://viewsourcecode.org/snaptoken/kilo/index.html
 * Left off on CH4
*/

#include "editor.h"
#include "rawMode.h"
#include "screenInfo.h"
#include <iostream>
#include <algorithm>

#define CTRL_KEY(k) ((k) & 0x1f)

enum EDITOR_KEYS  {
    BACKSPACE=127,
    ARROW_UP=1000,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    DELETE_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

const std::string helpMenuString = ""
"    __  ______    _   ______  ___  ____________\r\n"
"   /  |/  /   |  / | / / __ \\/   |/_  __/ ____/\r\n"
"  / /|_/ / /| | /  |/ / / / / /| | / / / __/   \r\n"
" / /  / / ___ |/ /|  / /_/ / ___ |/ / / /___   \r\n"
"/_/  /_/_/  |_/_/ |_/_____/_/  |_/_/ /_____/   \r\n"
"                                               \r\n"
"===============================================\r\n"
"                   Ver 1.0.1                   \r\n"
"                                               \r\n"
"Command Line Options:                          \r\n"
"mandate -h              Displays this help menu\r\n"
"                                               \r\n";

char* getCmdOption(char** begin, char** end, const std::string& option)  {
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)  {
        return *itr;
    }
    return nullptr;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)  {
    return std::find(begin, end, option) != end;
}


int readKeyInput();

int main(int argc, char** argv)  {
    enableRawMode();

    if (cmdOptionExists(argv, argv + argc, "-h"))  {

        std::cout << helpMenuString;
        //display help menu


        exit(0);
    }

    

    Editor textEditor;
    textEditor.load_from_file(argv[1]);

    while (true)  {
        // Display the text to the screen
        textEditor.display_lines();

        // Read input from keyboard
        int c = readKeyInput();

        switch (c)
        {
        // Quit
        case CTRL_KEY('q'):
            textEditor.save_to_file(argv[1]);
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        
        case CTRL_KEY('s'):
            textEditor.save_to_file(argv[1]);
            break;

        // Enter Key Pressed
        case '\r':
            textEditor.do_insert('\n');
            break;

        // Delete Key Pressed
        case DELETE_KEY:
            textEditor.do_delete();
            break;
        
        // Backspace Key Pressed
        case BACKSPACE:
            textEditor.do_backspace();
            break;

        // Cursor Movement
        case ARROW_LEFT:
            textEditor.cursor_left();
            break;
        case ARROW_RIGHT:
            textEditor.cursor_right();
            break;
        case ARROW_UP:
            textEditor.cursor_up();
            break;
        case ARROW_DOWN:
            textEditor.cursor_down();
            break;
        case PAGE_UP:
        case PAGE_DOWN:
            {
            int row, col;
            getWindowSize(&row, &col);
            while (col--)
                (c == PAGE_UP ? textEditor.cursor_up() : textEditor.cursor_down());
            }
            break;
        case HOME_KEY:
            
            break;
        case END_KEY:

            break;
        
        // Default is inserting a character
        default:
            textEditor.do_insert(c);
            break;
        }
    }

    return 0;
}

int readKeyInput()  {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    if (c == '\x1b')  {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {

            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DELETE_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else  {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        }
        return '\x1b'; 
    }
    return c;
}