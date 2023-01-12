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
#include <vector>
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
"          __  ______    _   ______  ___  ____________      \r\n"
"         /  |/  /   |  / | / / __ \\/   |/_  __/ ____/      \r\n"
"        / /|_/ / /| | /  |/ / / / / /| | / / / __/         \r\n"
"       / /  / / ___ |/ /|  / /_/ / ___ |/ / / /___         \r\n"
"      /_/  /_/_/  |_/_/ |_/_____/_/  |_/_/ /_____/         \r\n"
"                                                           \r\n"
"===========================================================\r\n"
"                        Ver 1.0.1                          \r\n"
"                                                           \r\n"
"Usage: mandate [OPTIONS] [FILE]                            \r\n"
"\r\n"
"Option      Long Option           Meaning                  \r\n"
"-h          --help                Displays this help menu  \r\n"
"                                                           \r\n";




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

    if (cmdOptionExists(argv, argv + argc, "-h") || cmdOptionExists(argv, argv + argc, "--help"))  {
        //display help menu
        std::cout << helpMenuString;
        exit(0);
    }

    

    int rows, cols;
    getWindowSize(&rows, &cols);

    Editor textEditor;
    textEditor.load_from_file(argv[1]);

    while (true)  {
        // Display the text to the screen
        //textEditor.display_lines(rows, cols);
        
        
        //Alt display to screen
        std::vector<std::string> outputLines = textEditor.get_display_lines(rows, cols);
        std::string displayOutput = "";
        displayOutput += "\x1b[?25l";   // Hide Cursor
        displayOutput += "\x1b[H";      // Repositions Cursor to top left of screen (for writing text properly to screen)
        for (int i = 0; i < rows; i++)  {
            displayOutput += "\x1b[K";  // Clear Old Line Contents
            displayOutput += outputLines[i];    // Put line content
            if (i < rows - 1)  { displayOutput += "\r\n"; } // Add newline if not the last line
        }
        // Get information about the cursor & scroll position
        size_t cursorRow = (textEditor.lines.size() > 0) ? textEditor.get_cursor_row() + 1 : 1;
        size_t cursorCol = (textEditor.lines.size() > 0) ? textEditor.get_cursor_col() + 1 : 1;
        int rowScroll = textEditor.get_scroll_row(rows);
        int colScroll = textEditor.get_scroll_col(cols);
        // Position Cursor to correct position
        char buf[32];
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (int)cursorRow - rowScroll, (int)cursorCol - colScroll); // Subtract scroll values here to prevent doubled movement speed
        displayOutput += buf;
        displayOutput += "\x1b[?25h"; // Show Cursor
        write(STDOUT_FILENO, displayOutput.c_str(), displayOutput.size()); // Write the entire sequence
        


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