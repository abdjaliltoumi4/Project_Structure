#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>


typedef struct Element
{
    int *character;
    size_t data_size;       // Size of the allocated memory
    size_t current_length;  // Current length of the buffer
    int pos;
    struct Element *suivent;
}Element;
typedef struct{
    Element *head;
    int sizeListe;
    Element *tail;
}ControlListe;
ControlListe *createListe();
void insertCharacterInLine(Element *line, int pos, int ch);
void insertLineInListe(ControlListe *l, int pos, Element *line);
void freeListe(ControlListe *liste);
Element *startNewLineBuffer();
size_t getBufferLength(Element *current_element);
void afficheLineListe(ControlListe *l);
void afficheline(Element *line);
#endif // MEMORY_H