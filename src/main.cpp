/**
 * Work on this in WSL
 * 
 * https://viewsourcecode.org/snaptoken/kilo/index.html
 * Left off on CH4
 * -> You need to implement a custom data structure and handle some other things in it better than the tutorial
 * -> C++ ify it with classes and stuff. Modularize stuff as appropriate
*/

#include "editor.h"
#include "rawMode.h"

int main()  {
    enableRawMode();
    Editor textEditor;

    while (true)  {
        int nread;
        char c;
        while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
            if (nread == -1 && errno != EAGAIN) die("read");
        }

        switch (c)
        {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;

        case 'a':
            textEditor.cursor_left();
            break;
        case 'd':
            textEditor.cursor_right();
            break;
        case 'w':
            textEditor.cursor_up();
            break;
        case 's':
            textEditor.cursor_down();
            break;
        
        default:
            textEditor.do_insert(c);
            break;
        }
    }

    //while (true)  {
    //    textEditor.refreshScreen();
    //    textEditor.processKeyPress();
    //}
    

    return 0;
}
