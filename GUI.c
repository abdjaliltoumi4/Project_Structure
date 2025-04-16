#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memory.h"
#include "handleKeys.h"

int min_width = 70;
int min_heigth = 8;
int NumberOfline = 1; // Number of line controled by the function createNewLines
//void checkScreenBounds(WINDOW *win);
void printContent(WINDOW *scr, ControlListe *l);


void setNumbersLine(WINDOW *scr, int number_line){
    int x, y;
    getyx(scr, y, x);
    if (y < 3){
        y = 3;
    }
    int prefix_width = NumberOfprifix(number_line) + 2;// plus 2 for space and t point
    wmove(scr, y, 0); // Move to the beginning of the line
    wprintw(scr, "%d. ", number_line); // Print the provided line number
    wmove(scr, y, prefix_width); // move to editable area
}

void createNewLines(WINDOW *scr, int ch){
    int y, x;
    getyx(scr, y, x); 
    if (ch == '\n'){
        NumberOfline++; // increment the number of lines 
        x = 0;
        wmove(scr, y + 1, x);
        setNumbersLine(scr, NumberOfline);
    }
}
// Function for set the color
void setColors(){
    if (has_colors() == TRUE){
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    }
    else{
        perror("the Terminal dosen't support COLORS");
        exit(1);
    }
}
// Function for draw the heafder and footer
void draw_header(WINDOW *win, const char *newName){
    int max_x, max_y;
    getmaxyx(win, max_y, max_x);
    mvwhline(win, 0, 0, ' ', max_x);
    mvwhline(win, 1, 0, ' ', max_x);

    wmove(win, 0, 0); // start from the left corner 
    // Header
    wattron(win, A_BOLD | COLOR_PAIR(1));
    for (size_t i = 0; i < (max_x - strlen(newName)) / 2; ++i){
        mvwprintw(win, 0, i, " ");
    }
    mvwprintw(win, 0, (max_x - strlen(newName)) / 2, "%s", newName);
    for (size_t i = (max_x - strlen(newName)) / 2 + strlen(newName); i < max_x; ++i){
        mvwprintw(win, 0, i, " ");
    }
    for (size_t i = 0; i < (max_x - strlen(" Crt+F Crt+S Crt+Z Press Crt+X to quit ")) / 2; ++i){
        mvwprintw(win, 1, i, " ");
    }
    mvwprintw(win, 1, (max_x - strlen(" Crt+F Crt+S Crt+Z Press Crt+X to quit ")) / 2, " Crt+F Crt+S Crt+Z Press Crt+X to quit ");
    for (size_t i = ((max_x - strlen(" Crt+F Crt+S Crt+Z Press Crt+X to quit ")) / 2) + strlen(" Crt+F Crt+S Crt+Z Press Crt+X to quit "); i < max_x; ++i){
        mvwprintw(win, 1, i, " ");
    }
    wattroff(win, A_BOLD | COLOR_PAIR(1));
    wmove(win, 3, 3); // move to editable area
}

int main(){
    const char *filename = "New File";

    // Initialize ncurses
    initscr();
    int tem_max_x, tem_max_y;
    getmaxyx(stdscr, tem_max_y, tem_max_x);
    if(tem_max_x <= min_width || tem_max_y <= min_heigth){
        endwin();
        fprintf(stdout,"the editor cannot diplay in thoe heigth and width!!! please reexecute the programme with more large window");
        exit(EXIT_FAILURE);
    }
    
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    // Set up colors
    setColors();
    
    // Initialize data structures
    ControlListe *liste = createListe();
    Element *current_buffer = startNewLineBuffer();

    // Draw initial screen
    setNumbersLine(stdscr, NumberOfline);
    draw_header(stdscr, filename);

    // Main input loop
    int ch;
    while ((ch = getch()) != 24){
        // if (!isValideKey(ch)){
        //     continue;
        // }
        
        if (ch == KEY_RESIZE){
            // Save the current buffer into the ControlListe before resizing
            if (getBufferLength(current_buffer) > 0){
                int *final_buffer = realloc(current_buffer->character, getBufferLength(current_buffer) * sizeof(int));
                if (final_buffer){
                    current_buffer->character = final_buffer;
                }
                insertLineInListe(liste, liste->sizeListe, current_buffer);
            }

            clear();
            int maxx, maxy;
            getmaxyx(stdscr, maxy, maxx);

            if(maxx <= min_width || maxy <= min_heigth){
                endwin();
                fprintf(stderr,"the editor cannot diplay in thoe heigth and width!!! please reexecute the programme with more large window");
                free(current_buffer->character);
                free(current_buffer);
                freeListe(liste);
                exit(EXIT_FAILURE);
            }
            
            draw_header(stdscr, filename);
            printContent(stdscr, liste);
            refresh();
        }
        else if (ch == '\n'){
            insertCharacterInLine(current_buffer, (int)current_buffer->current_length, '\n');
            // Save the current line content            
            insertLineInListe(liste, liste->sizeListe, current_buffer);

            //start a new buffer
            current_buffer = startNewLineBuffer();
            if (current_buffer == NULL){
                perror("Failed to allocate buffer for new line");
                break;
            }

            // Update the display
            createNewLines(stdscr, ch);
            refresh();
        }
        else if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_RIGHT || ch == KEY_LEFT){
            ArrowKeys(stdscr, ch, liste, current_buffer, NumberOfline);
        }
        else if (ch >= 32 && ch <= 126){
            HandlePrintableCharacter(stdscr, ch, NumberOfline, current_buffer, liste);
        }
    }
    endwin();
    // Cleanup
   if ( (delwin(stdscr) == ERR)){
    fprintf(stderr, "the delete window doesn't work correncty");
    exit(1);
   }
    free(current_buffer->character);
    free(current_buffer);
    freeListe(liste);
    
    return 0;
}

void printContent(WINDOW *scr, ControlListe *l){
    Element *cur = l->head;
    NumberOfline = 1; // reset the global variable
    if (l->head == NULL && l->tail == NULL){
        setNumbersLine(scr, NumberOfline);
    }
    
    
    wmove(scr, 3, 0); // move to editable area
    setNumbersLine(scr, NumberOfline);

    int max_y, max_x, y, x;
    getmaxyx(scr, max_y, max_x);
    getyx(scr, y, x);

    int prefix = NumberOfprifix(NumberOfline) + 2;
    while (cur != NULL){
        for (size_t i = 0; i < cur->current_length; ++i){
            if (cur->character[i] == '\n'){
                createNewLines(scr, cur->character[i]);
                prefix =  NumberOfprifix(NumberOfline) + 2;
            }
            else{
                mvwprintw(scr, y, (prefix + i), "%c", cur->character[i]);
            }
        }
        cur = cur->suivent;
    }
}

