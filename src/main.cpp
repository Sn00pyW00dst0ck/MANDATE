/**
 * Work on this in WSL
*/

#include "rawMode.h"

int main()  {
    enableRawMode();
    Editor textEditor;

    while (true)  {
        textEditor.refreshScreen();
        textEditor.processKeyPress();
    }

    return 0;
}
