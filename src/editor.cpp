#include "editor.h"
#include "rawMode.h"
#include "screenInfo.h"
#include <string>
#include <iostream>


void Editor::cursor_left()  {
    if (this->cursor > 0)  { this->cursor--; }
}
void Editor::cursor_right()  {
    if (this->cursor < this->data.size())  { this->cursor++; }
}
void Editor::cursor_up()  {
    size_t cursor_row = this->get_cursor_row();
    size_t cursor_col = this->get_cursor_col();
    if (cursor_row <= 0)  { return; }   // Do nothing if we can't move up

    // Move up and reposition cursor to end of line if needed
    Line next_line = this->lines[cursor_row - 1];
    size_t next_line_size = next_line.end - next_line.start;
    if (cursor_col > next_line_size) cursor_col = next_line_size;
    this->cursor = next_line.start + cursor_col;
}
void Editor::cursor_down()  {
    size_t cursor_row = this->get_cursor_row();
    size_t cursor_col = this->get_cursor_col();
    if (cursor_row >= this->lines.size() - 1)  { return; }   // Do nothing if we can't move down

    // Move down and repostition cursor to end of line if needed
    Line next_line = this->lines[cursor_row + 1];
    size_t next_line_size = next_line.end - next_line.start;
    if (cursor_col > next_line_size) cursor_col = next_line_size;
    this->cursor = next_line.start + cursor_col;
}


size_t Editor::get_cursor_row()  {
    assert(this->lines.size() > 0);
    for (size_t i = 0; i < this->lines.size(); i++)  {
        Line curr_line = this->lines[i];
        if (curr_line.start <= this->cursor && this->cursor <= curr_line.end)  { return i; }
    }
    return this->lines.size() - 1;
}
size_t Editor::get_cursor_col()  {
    return this->cursor - this->lines[this->get_cursor_row()].start;
}


void Editor::do_backspace()  {
    // Might need special case handling here
    this->data.erase(this->data.begin() + cursor - 1);
    this->cursor--;
    this->recompute_lines();
}
void Editor::do_delete()  {
    // Might need special case handling here
    this->data.erase(this->data.begin() + cursor);
    this->recompute_lines();
}
void Editor::do_insert(char c)  {
    // Might need special case handling here    
    this->data.insert(cursor, 1, c);
    this->cursor++;
    this->recompute_lines();
}
void Editor::recompute_lines()  {
    this->lines.clear();
    Line curr_line;
    curr_line.start = 0;

    for (size_t i = 0; i < this->data.size(); i++)  {
        if (this->data[i] == '\n')  {
            curr_line.end = i;
            this->lines.push_back(curr_line);
            curr_line.start = i + 1;
        }
    }

    curr_line.end = this->data.size();
    this->lines.push_back(curr_line);
}


void Editor::save_to_file(const char *file_path)  {

}
void Editor::load_from_file(const char *file_path)  {

}
void Editor::get_file_size(const char *file_path)  {

}

void Editor::display_lines()  {
    // Get information about the terminal
    int rows, cols;
    getWindowSize(&rows, &cols);

    std::string displayOutput = "";
    displayOutput += "\x1b[?25l";   // Hide Cursor
    displayOutput += "\x1b[H";      // Repositions Cursor to top left of screen (for writing text properly to screen)

    int pos = 0;
    for (int y = 0; y < rows; y++)  {
        displayOutput += "\x1b[K";  // Clear Old Line Contents

        if (y >= this->lines.size() || this->lines[y].start == this->lines[y].end)  {
            displayOutput += "~";
        } else  {
            // Print out this line's contents
            displayOutput += this->data.substr(this->lines[y].start, this->lines[y].end - this->lines[y].start);
        }
        
        if (y < rows - 1)  { displayOutput += "\r\n"; }
    }

    // Position Cursor to correct position
    char buf[32];
    if (this->lines.size() > 0)  {
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (int)this->get_cursor_row() + 1, (int)this->get_cursor_col() + 1);
    } else  {
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", 1, 1);
    }
    displayOutput += buf;
    displayOutput += "\x1b[?25h"; // Show Cursor

    write(STDOUT_FILENO, displayOutput.c_str(), displayOutput.size()); // Write the entire sequence
}

/*
//==============================CONSTRUCTORS & DESTRUCTORS==============================//

Editor::Editor()  {
    cursorX = 0;
    cursorY = 0;
    //Get Window Size
    if (getWindowSize(&screenRows, &screenCols) == -1) die("getWindowSize");
}

Editor::~Editor()  {

}

//==============================PRIVATE METHODS==============================//

int Editor::readKey()  {
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

void Editor::moveCursor(int key)  {
    switch (key) {
        case ARROW_LEFT:
            if (cursorX != 0)  { cursorX--; }
            break;
        case ARROW_RIGHT:
            if (cursorX != screenCols - 1)  { cursorX++; }
            break;
        case ARROW_UP:
            if (cursorY != 0)  { cursorY--; }
            break;
        case ARROW_DOWN:
            if (cursorY != screenRows - 1)  { cursorY++; }
            break;
    }
}



void Editor::drawRows(struct writeBuffer *wb)  {
    for (int y = 0; y < screenRows; y++)  {
        wbAppend(wb, "~", 1);
        wbAppend(wb, "\x1b[K", 3);
        if (y < screenRows - 1)  { wbAppend(wb, "\r\n", 2); }
    }
}


//==============================PUBLIC METHODS==============================//

void Editor::refreshScreen()  {
    // Need to make write buffer stuff
    struct writeBuffer wb = WBUF_INIT; // Write Buffer

    wbAppend(&wb, "\x1b[?25l", 6); // Hide Cursor
    wbAppend(&wb, "\x1b[H", 3);  // Repositions Cursor to top left of screen (for writing text properly to screen)

    drawRows(&wb);

    // Position Cursor to old user position
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", cursorY + 1, cursorX + 1);
    wbAppend(&wb, buf, strlen(buf));

    wbAppend(&wb, "\x1b[?25h", 6); // Show Cursor

    write(STDOUT_FILENO, wb.b, wb.len); // Write the entire sequence
    wbFree(&wb); // Free the buffer
}

void Editor::processKeyPress()  {
    int c = this->readKey();

    switch (c)  {
    case CTRL_KEY('q'):
        // Reset Screen And Exit
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    
    case DELETE_KEY:
        break;
    
    case HOME_KEY:
      cursorX = 0;
      break;
    case END_KEY:
      cursorX = screenCols - 1;
      break;
    
    case PAGE_UP:
    case PAGE_DOWN:
        {
            int times = screenRows;
            while (times--)
                moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        }
        break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
        moveCursor(c);
        break;
    default:
        break;
    }
}
*/