#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>
#include "menu.h"
#include "auth.h"
#include "game.h"

int main() {
    initscr();             
    noecho();              
    curs_set(FALSE);
    keypad(stdscr, TRUE); 

    

    if (has_colors()){
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_CYAN);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_YELLOW);
    }      

    // گرفتن ابعاد ترمینال
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  

    // نوشتن اسم بازی در شروع
    game_name(rows, cols);

    int selected = 0; // گزینه انتخاب شده در منو
    int num_items = 3;
    Menu(rows, cols, &selected, num_items);    
                       
    endwin();             
    return 0;
}