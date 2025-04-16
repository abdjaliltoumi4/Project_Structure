#ifndef GUI_H
#define GUI_H
#include <ncurses.h>
#include "memory.h"

void printContent(WINDOW *scr, ControlListe *l);
WINDOW *handle_resize(WINDOW *win, const char *newName, ControlListe *l);
void setNumbersLine(WINDOW *scr, int ch);
void setFirsteNumber(WINDOW *scr);
void createNewLines(WINDOW *scr, int ch);
void setColors();
void draw_header(WINDOW *win, const char*newName);
#endif // GUI_H