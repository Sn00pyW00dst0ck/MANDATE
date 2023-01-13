
#include "editorManager.h"
#include "screenInfo.h"

EditorManager::EditorManager()  {

}

EditorManager::~EditorManager()  {

}


void EditorManager::openNewEditor(const char* file_path)  {
    this->editors.push_back({});
    this->editors.back().load_from_file(file_path);
    this->activeEditor = this->editors.size() - 1;
}

Editor& EditorManager::getActiveEditor()  {
    return this->editors[this->activeEditor];
}

void EditorManager::swapActiveEditor(int index)  {
    if (index >= 0 && index < this->editors.size())  {
        this->activeEditor = index;
    }
}

void EditorManager::saveActiveEditorToFile(const char* file_path)  {
    this->getActiveEditor().save_to_file(file_path);
}

void EditorManager::closeActiveEditor()  {
    this->editors.erase(this->editors.begin() + this->activeEditor);
    while (this->activeEditor >= this->editors.size())  {
        this->activeEditor--;
    }
}


// Handle Key Presses Here



void EditorManager::displayEditors()  {
    std::string displayOutput = "";
    displayOutput += "\x1b[?25l";   // Hide Cursor
    displayOutput += "\x1b[H";      // Repositions Cursor to top left of screen (for writing text properly to screen)
    

    //Get screen display information
    int rows, cols;
    getWindowSize(&rows, &cols);

    //Calculate how big each editor should be width and height wise (for now equal sizes)
    cols /= this->editors.size();
    cols -= this->editors.size();

    // Placeholder just display the active editor at all times
    std::vector<std::string> outputLines = this->getActiveEditor().get_display_lines(rows, cols);

    // Get the output lines from each editor
    std::vector<std::string> outs;
    std::vector<std::vector<std::string>> editorOutputLines;
    for (int i = 0; i < this->editors.size(); i++)  {
        editorOutputLines.push_back(this->editors[i].get_display_lines(rows, cols));
    }
    for (int y = 0; y < rows; y++)  {
        outs.push_back("");
        for (int i = 0; i < this->editors.size(); i++)  {
            outs[y] += editorOutputLines[i][y];
            if (i < this->editors.size() - 1)  { outs[y] += "║"; }
        }
    }

    for (int i = 0; i < rows; i++)  {
        displayOutput += "\x1b[K";  // Clear Old Line Contents
        displayOutput += outs[i];   // Write the calculated line content
        if (i < rows - 1)  { displayOutput += "\r\n"; } // Add newline if not the last line
    }

    // TO DO: Clean up cursor position calculation

    // Get information about the cursor & scroll position
    size_t cursorRow = (this->getActiveEditor().lines.size() > 0) ? this->getActiveEditor().get_cursor_row() + 1 : 1;
    size_t cursorCol = (this->getActiveEditor().lines.size() > 0) ? this->getActiveEditor().get_cursor_col() + 1 : 1;
    int rowScroll = this->getActiveEditor().get_scroll_row(rows);
    int colScroll = this->getActiveEditor().get_scroll_col(cols);

    // Position within editor window + position of editor window (clean this up)
    int finalCursorCol = (cursorCol - colScroll) + (this->activeEditor * (1 + editorOutputLines[0][0].size()));

    // Position Cursor to correct position
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (int)cursorRow - rowScroll, finalCursorCol); // Subtract scroll values here to prevent doubled movement speed
    displayOutput += buf;
    displayOutput += "\x1b[?25h"; // Show Cursor
    write(STDOUT_FILENO, displayOutput.c_str(), displayOutput.size()); // Write the entire sequence
}