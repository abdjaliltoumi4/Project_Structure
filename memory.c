
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include "memory.h"

#define INITIAL_SIZE 2   // Initial size of the memory block
#define INCREMENT_SIZE 2 // How much to increase memory by each time

// create liste
ControlListe *createListe()
{
    ControlListe *liste = (ControlListe *)malloc(sizeof(ControlListe));
    if (liste == NULL)
    {
        perror("Memory allocation for liste Fail");
        exit(1);
    }
    liste->head = NULL;
    liste->tail = NULL;
    liste->sizeListe = 0;
    return liste;
}

// for freeing the liste and its content
void freeListe(ControlListe *liste){
    if (!liste)
        return;
    Element *current = liste->head;
    Element *next;
    while (current != NULL){
        next = current->suivent;
        free(current->character); // Free the string data for this node
        free(current);            // Free the list node itself
        current = next;
    }
    free(liste); // Free the list control structure
}

// Returns the line at the given index in the list (0-indexed)
// If index is out of bounds, returns NULL.
Element *getLineAt(ControlListe *liste, int index){
    if(index < 0 || index >= liste->sizeListe)
        return NULL;
    Element *cur = liste->head;
    int pos = 0;
    while(cur != NULL && pos < index){
        cur = cur->suivent;
        pos++;
    }
    return cur;
}

// start new buffer (new line)
Element *startNewLineBuffer(){
    Element *new_element = (Element *)malloc(sizeof(Element));
    if (new_element == NULL)
    {
        perror("Memory allocation for new element failed");
        exit(1);
    }

    new_element->data_size = INITIAL_SIZE;
    new_element->current_length = 0;
    new_element->character = (int *)malloc(new_element->data_size * sizeof(int));
    if (new_element->character == NULL)
    {
        perror("Memory allocation for buffer failed");
        free(new_element);
        exit(1);
    }

    new_element->suivent = NULL;
    return new_element;
}

size_t getBufferLength(Element *current_element)
{
    if (current_element == NULL)
    {
        fprintf(stderr, "getBufferLength received a NULL pointer\n");
        return 0;
    }
    return current_element->current_length;
}

void insertCharacterInLine(Element *line, int pos, int ch){
    if (!line){
        fprintf(stderr, "Error: NULL line pointer passed to insertCharacterInLine.\n");
        exit(1);
    }
    // Allow insertion at the end (pos == current_length) or in the middle.
    if (pos < 0 || pos > (int)line->current_length)
    {
        fprintf(stderr, "Position %d is out of bounds (current length: %zu) for insertCharacterInLine\n", pos, line->current_length);
        exit(1);
    }

    if (line->current_length + 1 >= line->data_size){
        size_t new_size = line->data_size + INCREMENT_SIZE;
        if (new_size <= line->current_length){
            new_size = line->current_length + 1;
        }

        int *temp = realloc(line->character, new_size * sizeof(int));
        if (temp == NULL)
        {
            perror("Memory reallocation failed in insertCharacterInLine");
            exit(1);
        }
        line->character = temp;
        line->data_size = new_size;
    }

    // Shift characters rightward if inserting in the middle.
    if (pos < (int)line->current_length){
        memmove(&line->character[pos + 1],
                &line->character[pos],
                (line->current_length - pos) * sizeof(int));
    }

    line->character[pos] = ch;
    line->current_length++;
}

void insertLineInListe(ControlListe *l, int pos, Element *line)
{
    if (pos < 0 || pos > l->sizeListe)
    { // Ensure position is valid
        fprintf(stderr, "Position is out of bounds in insertLineInListe\n");
        exit(1);
    }

    if (l->head == NULL)
    {
        // If the list is empty, set the new line as both head and tail.
        l->head = line;
        l->tail = line;
        line->pos = 0;
    }
    else if (pos == 0){
        // Insert at the beginning of the list.
        line->suivent = l->head;
        l->head = line;
        line->pos = 0;
    }
    else{
        // Insert at the specified position.
        line->pos = pos;
        Element *cur = l->head;
        int current_pos = 0;
        while (cur != NULL && current_pos < pos - 1)
        { // find the element before pos.
            cur = cur->suivent;
            current_pos++;
        }
        if (cur != NULL)
        {
            line->suivent = cur->suivent;
            cur->suivent = line;
            if (line->suivent == NULL)
            {
                l->tail = line; // Update tail if inserted at the end.
            }
        }
    }
    l->sizeListe++;
}

void afficheLineListe(ControlListe *l)
{
    if (l->tail == NULL && l->head == NULL)
    {
        fprintf(stderr, "The liste is empty in afficheLineListe\n");
        exit(1);
    }
    Element *cur = l->head;
    while (cur != NULL)
    {
        int *curCara = cur->character;
        while ((*curCara) != '\n' && curCara - cur->character < (int)cur->current_length)
        {
            printf("%c", (*curCara));
            curCara++;
        }
        printf("\n");
        cur = cur->suivent;
    }
}

void afficheline(Element *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "line is empty\n");
        return;
    }
    int *curCara = line->character;
    while ((*curCara) != '\n' && curCara - line->character < (int)line->current_length)
    {
        printf("%c", (*curCara));
        curCara++;
    }
    printf("\n");
}

// Helper to clear the current terminal line and reprint the provided line's content.
// void refreshEditingLine(Element *editLine) {
//     printf("\r\033[K"); // Return to beginning of line and clear it.
//     for (int i = 0; i < (int)editLine->current_length; i++){
//         printf("%c", editLine->character[i]);
//     }
//     fflush(stdout);
// }
/*
int main(){
    // Save the current terminal settings.
    struct termios old_attr, new_attr;
    if (tcgetattr(STDIN_FILENO, &old_attr) != 0) {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }
    new_attr = old_attr;
    // Disable canonical mode and echo.
    new_attr.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_attr) != 0) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }

    ControlListe *liste = createListe();
    printf("==================================================================\n");
    printf("Pour sortir le programme, appuyez sur q ou Q\n");
    printf("==================================================================\n");

    // newLineBuffer is the buffer used for new (unsaved) lines.
    Element *newLineBuffer = startNewLineBuffer();
    // editingLine points to the current line being edited.
    Element *editingLine = newLineBuffer;
    // currentLineIndex tracks the edited line's index:
    // if equals liste->sizeListe, it indicates the new (unsaved) line.
    int currentLineIndex = liste->sizeListe; 
    int ch;
    int cursor = editingLine->current_length; // current insertion position

    while ((ch = getchar()) != 'q' && ch != 'Q') {
        // Handle arrow keys (they send an escape sequence)
        if(ch == 27){
            int next1 = getchar();
            int next2 = getchar();
            if(next1 == '['){
                // Right arrow (ESC [ C)
                if(next2 == 'C'){
                    if(cursor < (int)editingLine->current_length){
                        cursor++;
                        printf("\033[C"); // Move terminal cursor right
                    }
                }
                // Left arrow (ESC [ D)
                else if(next2 == 'D'){
                    if(cursor > 0){
                        cursor--;
                        printf("\033[D"); // Move terminal cursor left
                    }
                }
                // Up arrow (ESC [ A) - load previous line for editing.
                else if(next2 == 'A'){
                    if(currentLineIndex > 0){
                        currentLineIndex--;
                        // Load the line from the liste.
                        Element *loaded = getLineAt(liste, currentLineIndex);
                        if(loaded != NULL){
                            editingLine = loaded;
                            cursor = editingLine->current_length;
                            refreshEditingLine(editingLine);
                        }
                    } else {
                        printf("\a"); // beep if no previous line
                    }
                }
                // Down arrow (ESC [ B) - load next line for editing.
                else if(next2 == 'B'){
                    // If we are not at the new unsaved line, allow moving down.
                    if(currentLineIndex < liste->sizeListe){
                        currentLineIndex++;
                        if(currentLineIndex == liste->sizeListe){
                            // Switch to the new line buffer.
                            editingLine = newLineBuffer;
                        }
                        else{
                            editingLine = getLineAt(liste, currentLineIndex);
                        }
                        cursor = editingLine->current_length;
                        refreshEditingLine(editingLine);
                    }
                    else {
                        printf("\a"); // beep if already at the new line.
                    }
                }
            }
            continue; // Do not insert any character for escape sequences
        }
        // Handle Enter: finish editing the current line.
        if(ch == '\n' || ch == '\r'){
            // If currently editing the new (unsaved) line, insert it.
            if(currentLineIndex == liste->sizeListe){
                // Append a newline character to mark end-of-line.
                insertCharacterInLine(editingLine, editingLine->current_length, '\n');
                insertLineInListe(liste, liste->sizeListe, editingLine);
                printf("\n");
                // Create a new unsaved buffer.
                newLineBuffer = startNewLineBuffer();
            } else {
                // When editing a saved line, simply finish editing.
                printf("\n");
            }
            // Switch to new line buffer for next editing.
            editingLine = newLineBuffer;
            currentLineIndex = liste->sizeListe;
            cursor = editingLine->current_length;
            continue;
        }
        // Insert normal character at the current cursor position.
        insertCharacterInLine(editingLine, cursor, ch);
        // Reprint from the insertion point to the end.
        for (int i = cursor; i < (int)editingLine->current_length; i++){
            printf("%c", editingLine->character[i]);
        }
        // Move the terminal cursor back to the correct insertion position.
        int tailLength = editingLine->current_length - cursor - 1;
        for (int i = 0; i < tailLength; i++){
            printf("\033[D");
        }
        cursor++; // Update internal cursor position.
        fflush(stdout);
    }

    // Restore the original terminal settings.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old_attr) != 0) {
        perror("tcsetattr restore");
        exit(EXIT_FAILURE);
    }

    return 0;
}
*/