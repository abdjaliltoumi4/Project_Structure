#ifndef HANDLE_KEYS_H
#define HANDLE_KEYS_H
#include <ncurses.h>
#include "memory.h"
bool isValideKey(int ch);
void ArrowKeys(WINDOW *win, int ch, ControlListe *l, Element *current_buffer, int NumberOfline);
void HandlePrintableCharacter(WINDOW *win, int ch, int NumberOfline, Element *current_buffer, ControlListe *l);
int NumberOfprifix(int n);
void HandlePrintableCharacter(WINDOW *win, int ch, int NumberOfline, Element *current_buffer, ControlListe *l);
#endif // HANDLE_KEY