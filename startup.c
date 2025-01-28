#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "startup.h"

int rows, cols;


void game_name(int rows, int cols){
    attron(A_BOLD);
    char message[] = "Welcome to ROGUE!";
    mvprintw((rows / 2)-1, (cols - strlen(message)) / 2, "%s", message); // نمایش پیام در مرکز
    attroff(A_BOLD);

    char credit[] = "Made by Atena :)";
    mvprintw((rows/2)+1 , (cols - strlen(credit))/2 ,"%s", credit);
    refresh();                             
    sleep(2);                         
    clear(); 
}

void print_border(int rows, int cols){
    
    for (int i = 0; i < cols; i++) {
        mvaddch(0, i, '=');            // خط بالا
        mvaddch(rows - 1, i, '=');     // خط پایین
    }
    for (int i = 0; i < rows; i++) {
        mvaddstr(i, 0, "||");            // ستون سمت چپ
        mvaddstr(i, cols - 2, "||");     // ستون سمت راست
    }
    refresh();
}

void startup(){
    setlocale(LC_ALL, "");
    srand(time(NULL));
    initscr();             
    noecho();
    cbreak();              
    curs_set(FALSE);
    keypad(stdscr, TRUE); 
    if (has_colors()){
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_CYAN);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_YELLOW);
    }
    // نوشتن اسم بازی در شروع
    game_name(rows, cols);
}

// گرفتن ابعاد ترمینال
    