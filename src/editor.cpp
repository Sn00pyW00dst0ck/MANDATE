#include "editor.h"
#include "rawMode.h"
#include "screenInfo.h"
#include <string>
#include <iostream>
#include <fstream>


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
size_t Editor::get_scroll_row(int window_rows)  {
    size_t cursorRow = (this->lines.size() > 0) ? this->get_cursor_row() + 1 : 1;
    return (int)cursorRow > (window_rows / 2) ? (int)cursorRow - (window_rows / 2) : 0;
}
size_t Editor::get_scroll_col(int window_cols)  {
    size_t cursorCol = (this->lines.size() > 0) ? this->get_cursor_col() + 1 : 1;
    return (int)cursorCol > (window_cols / 2) ? (int)cursorCol - (window_cols / 2) : 0;
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
    // Open the output file
    std::ofstream fileSaver;
    fileSaver.open(file_path);
    if (!fileSaver)  { return; }

    // Write to the file and close it
    fileSaver << this->data;
    fileSaver.close();
}
void Editor::load_from_file(const char *file_path)  {
    // Open the input file
    std::ifstream fileReader;
    fileReader.open(file_path);
    if (!fileReader)  { return; }

    // Read the entire file content into the data array of the editor
    fileReader.seekg(0, std::ios::end);
    this->data.resize(fileReader.tellg());
    fileReader.seekg(0, std::ios::beg);
    fileReader.read(&this->data[0], this->data.size());
    fileReader.close();

    // Generate each line        
    this->recompute_lines();
}


std::vector<std::string> Editor::get_display_lines(int window_rows, int window_cols)  {
    // Get scroll information
    int rowScroll = this->get_scroll_row(window_rows);
    int colScroll = this->get_scroll_col(window_cols);

    std::vector<std::string> display_lines(window_rows);

    // Generate lines to be displayed
    for (int y = 0; y < window_rows; y++)  {
        int currentRow = y + rowScroll; // Offset by the vertical scroll value here
        // Get this line's contents
        if (!(currentRow >= this->lines.size() || this->lines[currentRow].start == this->lines[currentRow].end))  {
            // Determine part of line to write based on the horizontal scroll we calculated
            int len = (this->lines[currentRow].end - this->lines[currentRow].start) - colScroll;
            len = (len > window_cols) ? window_cols : len;
            if (len > 0)  { display_lines[y] = (this->data.substr(this->lines[currentRow].start + colScroll, len)); }
            else { display_lines[y] = (""); }
        }
        // Ensure all line's are same length (for consistency)
        while (display_lines[y].length() < window_cols)  {
            display_lines[y] += " ";
        }
    }
    // Pad ending of strings with spaces here (to make output consistent!)
    return display_lines;
}

void Editor::display_lines(int window_rows, int window_cols)  {
    // Get information about the cursor position
    size_t cursorRow = (this->lines.size() > 0) ? this->get_cursor_row() + 1 : 1;
    size_t cursorCol = (this->lines.size() > 0) ? this->get_cursor_col() + 1 : 1;

    // Calculate scroll information
    int rowScroll = (int)cursorRow > (window_rows / 2) ? (int)cursorRow - (window_rows / 2) : 0;
    int colScroll = (int)cursorCol > (window_cols / 2) ? (int)cursorCol - (window_cols / 2) : 0;

    std::string displayOutput = "";
    displayOutput += "\x1b[?25l";   // Hide Cursor
    displayOutput += "\x1b[H";      // Repositions Cursor to top left of screen (for writing text properly to screen)

    for (int y = 0; y < window_rows; y++)  {
        int currentRow = y + rowScroll; // Offset by the vertical scroll value here

        displayOutput += "\x1b[K";  // Clear Old Line Contents

        // Print out this line's contents
        if (!(currentRow >= this->lines.size() || this->lines[currentRow].start == this->lines[currentRow].end))  {
            // Determine part of line to write based on the scroll we calculated
            int len = (this->lines[currentRow].end - this->lines[currentRow].start) - colScroll;
            len = (len > window_cols) ? window_cols : len;
            if (len > 0)  { displayOutput += this->data.substr(this->lines[currentRow].start + colScroll, len); }
        }
        
        if (y < window_rows - 1)  { displayOutput += "\r\n"; }
    }

    // Position Cursor to correct position
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (int)cursorRow - rowScroll, (int)cursorCol - colScroll); // Subtract scroll values here to prevent doubled movement speed
    displayOutput += buf;
    displayOutput += "\x1b[?25h"; // Show Cursor

    write(STDOUT_FILENO, displayOutput.c_str(), displayOutput.size()); // Write the entire sequence
}
