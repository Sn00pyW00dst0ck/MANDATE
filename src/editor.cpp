#include "editor.h"
#include "rawMode.h"
#include "screenInfo.h"

void wbAppend(struct writeBuffer *wb, const char *s, int len) {
  char *newBuf = (char*)realloc(wb->b, wb->len + len);

  if (newBuf == NULL) return;
  memcpy(&newBuf[wb->len], s, len);
  wb->b = newBuf;
  wb->len += len;
}
void wbFree(struct writeBuffer *wb) {
  free(wb->b);
}



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
