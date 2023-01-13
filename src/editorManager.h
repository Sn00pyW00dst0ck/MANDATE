#pragma once

#include "editor.h"
#include <vector>

class EditorManager  {
public:
    int activeEditor = 0;
    std::vector<Editor> editors;

    EditorManager();
    ~EditorManager();

    Editor& getActiveEditor();
    void swapActiveEditor(int index);
    void closeActiveEditor();

    void openNewEditor(const char* file_path);
    void saveActiveEditorToFile(const char* file_path);


    void displayEditors();
};