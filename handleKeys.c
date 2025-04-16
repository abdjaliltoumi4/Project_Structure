#include "handleKeys.h"
#include <ncurses.h>
#include "GUI.h"
#include "memory.h"
// The number of character in that disappear in the right and left 
int num_chara_shift_to_left = 0;
int num_chara_shift_to_right = 0;
int line_number_offset = 3; // Your line number offset (header lines)
int line_number_width = 3;  // Width of "N. " prefix (adjust if needed)int NumberOfprifix(int n){
void shift_characher_to_letf(WINDOW *win, int line_prefix_width, int ch);
void shift_characher_to_right(WINDOW *win, Element *line);

int NumberOfprifix(int n){
    int i = 1;
    while ((n = n / 10) != 0)
    {
        i++;
    }
    return i;
}
bool isValideKey(int ch)
{
    if (ch >= KEY_F(1) && ch <= KEY_F(12))
    { // Function keys F1 to F12
        return FALSE;
    }
    else if (ch >= 32 && ch <= 126)
    { // Printable ASCII characters
        return TRUE;
    }
    else if (ch == KEY_HOME || ch == KEY_END)
    {
        return FALSE;
    }
    else if (ch == KEY_ENTER || ch == '\n' || ch == KEY_BACKSPACE)
    { // Enter key
        return TRUE;
    }
    else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)
    {
        return TRUE;
    }
    else if (ch == KEY_DC || ch == KEY_IC || ch == KEY_PPAGE || ch == KEY_NPAGE)
    { // Delete, Insert, Page Up, Page Down
        return FALSE;
    }
    else if (ch >= 1 && ch <= 26)
    {                                                         // Control characters (Ctrl+A to Ctrl+Z)
        return (ch == 6 || ch == 19 || ch == 26 || ch == 24); // Allow Ctrl+F, Ctrl+S, Ctrl+Z, Crt+ X
    }
    return FALSE;
}

Element *getElementAtline(ControlListe *l, int pos){
    if (l == NULL || l->head == NULL || pos < 0){
        return NULL;
    }
    if (pos >= l->sizeListe)
    { // Check bounds
        return NULL;
    }

    Element *cur = l->head;
    while (cur != NULL)
    {
        if (cur->pos == pos)
        {
            return cur;
        }
        cur = cur->suivent;
    }
    return NULL; // Not found
}

// handle the arrow keys
void ArrowKeys(WINDOW *win, int ch, ControlListe *l, Element *current_buffer, int NumberOfline){
    line_number_width = (NumberOfprifix(NumberOfline) + 2);
    int y, x;
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    getyx(win, y, x);

    size_t lineLength = 0;
    Element *currentElement = NULL;

    switch (ch)
    {
    case KEY_UP:
        int previous_line = y - 1;
        previous_line = previous_line - 2;
        if (y > line_number_offset)
        { // Don't move above editable line
            if (NumberOfprifix(previous_line) == 2){ 
                // + 1 for 0-based to 1-based
                line_number_width = 4;
                wmove(win, y - 1, 4);
            }
            else
            {
                line_number_width = 3;
                wmove(win, y - 1, 3);
            }
        }
        break;
    case KEY_DOWN:
        if (y < NumberOfline - 1 + line_number_offset && y < maxy - 1)
        { // Don't move down last line
            int next_line = y + 1;
            next_line = next_line - 2;
            if (NumberOfprifix((next_line)) == 2)
            { // for 0-based to 1-based
                line_number_width = 4;
                wmove(win, y + 1, 4);
            }
            else{
                line_number_width = 3;
                wmove(win, y + 1, 3);
            }
        }
        break;
    case KEY_RIGHT:
        int cur_line = y - 2;
        if (NumberOfprifix(cur_line) == 2){
            line_number_width = 4;
        }
        else{
            line_number_width = 3;
        }

        int pos = y - line_number_offset;
        currentElement = getElementAtline(l, pos);
        if (currentElement == NULL){
            if ((pos + 1) == NumberOfline){
                if (x < current_buffer->current_length + line_number_width && x < maxx -2){
                    // don't move rigth more the editable area
                    wmove(win, y, x + 1);
                }
            }
        }
        else{
            lineLength = getBufferLength(currentElement);
            if (x < ((int)lineLength) + line_number_width - 1 && x < maxx - 2) { 
                // -1 for the new line charater that already stored with each line
                wmove(win, y, x + 1);
            }
        }
        break;
    case KEY_LEFT:
        cur_line = y - 2;
        if (NumberOfprifix(cur_line) == 2){
            line_number_width = 4;
        }
        else{
            line_number_width = 3;
        }        
        if(x > line_number_width){
            if(num_chara_shift_to_left > 0 && x == 3){
                fprintf(stderr, "this is work");
                shift_characher_to_right(win, current_buffer);
            }
            else{
                // Don't move left of the line number width Prefix
                wmove(win, y, x - 1);
            }
        }
        break;
    default:
        break;
    }
    wrefresh(win); // Refresh window after moving cursor
}

void printLineContent(WINDOW *win, Element *line, int line_prefix_width){
    if (win == NULL || line == NULL || line->character == NULL)
    {
        fprintf(stderr, "printLineContent recive a NULL line or win");
        exit(1);
    }
    int max_y, max_x, current_y, original_x;
    getmaxyx(win, max_y, max_x);
    getyx(win, current_y, original_x); // Store original cursor position

    // Clear the line from the content area
    wmove(win, current_y, line_prefix_width);
    wclrtoeol(win);

    // Print characters from the buffer
    wmove(win, current_y, line_prefix_width);
    for (int i = 0; i < line->current_length; ++i)
    {
        if (line->character[i] == '\n'){
            break; // Stop at terminator
        }
        int current_print_x = line_prefix_width + i;
        if (current_print_x >= max_x){
            break; // Stop at window edge
        }
        // display the character
        mvwaddch(win, current_y, (line_prefix_width + i), (unsigned char)line->character[i]);
    }

    // Restore the original cursor position
    wmove(win, current_y, original_x+1);
}
// function for check bounsers and its shift the character to left 
void shift_characher_to_letf(WINDOW *win, int line_prefix_width, int ch){
    int x, y, maxx, maxy;
    getyx(win, y, x);
    getmaxyx(win, maxy, maxx);


    if(x >= maxx - 1){
        // if cursor at the end of line 
        for (size_t i = line_prefix_width; i < maxx - 1; ++i){
            int ch = mvwinch(win, y, (i+1));
            mvwaddch(win, y, i, ch);
            wmove(win, y, (i+1));
        }
        num_chara_shift_to_left++;
        wmove(win, y, (x-1));
    }
    else if (ch >= 32 && ch <= 126){
        // the cursor is not at the end display the character
        waddch(win, ch);
        wmove(win, y, x+1);
    }
}
// function for check bounsers and its shift the character to right<<<<<<<<<
void shift_characher_to_right(WINDOW *win, Element *line){
    int maxx, maxy, y, x;
    getmaxyx(win, maxy, maxx);
    getyx(win, y, x);

    if (num_chara_shift_to_left > 0 && x <= 3){
        wclrtoeol(win);
        wmove(win, y, x);
        // -1 for based 1 to 0
        for(size_t i = (num_chara_shift_to_left-1); i < maxx - 1; i++){
            waddch(win, (unsigned char)line->character[i]);
            wmove(win, y, x+1);
        }
        wmove(win, y, x);
    }
}
// hanle the printable character and the editing in existing line
void HandlePrintableCharacter(WINDOW *win, int ch, int NumberOfline, Element *current_buffer, ControlListe *l){
    line_number_width = (NumberOfprifix(NumberOfline) + 2); // plus 2 for point and space
    int x, y, max_x, max_y;
    getyx(win, y, x);
    getmaxyx(win, max_y, max_x);
    // Handle printable characters for safty
    if (ch >= 32 && ch <= 126){
        int posLine = y - line_number_offset;
        // int posChar = x - line_number_width;
        Element *currentElement = getElementAtline(l, posLine);
        if (currentElement == NULL){
            // this condition if the user edite on the current buffer
            if ((posLine + 1) == NumberOfline){ 
                int pos_in_buffer = x - line_number_width; 
                // when user edite the current buffer
                if (pos_in_buffer < (int)current_buffer->current_length){
                    // when user reach the end of line
                    if (x >= (max_x - 2 )){
                        insertCharacterInLine(current_buffer, current_buffer->current_length, ch);
                        shift_characher_to_letf(win, line_number_width, ch);
                    }
                    else{
                        insertCharacterInLine(current_buffer, pos_in_buffer, ch);
                        printLineContent(win, current_buffer, line_number_width);
                    }
                }
                else{
                    insertCharacterInLine(current_buffer, current_buffer->current_length, ch);
                    shift_characher_to_letf(win, line_number_width, ch);
                }
            }
        }
        // if the user edite on the anther line or buffer
        else
        {
            int posChar = x - line_number_width;
            insertCharacterInLine(currentElement, posChar, ch);
            printLineContent(win, currentElement, line_number_width);
        }
    }
    refresh();
}