#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>
#include "menu.h"
#include "auth.h"
#include "game.h"
#include "startup.h"

int main() {
    startup();
    getmaxyx(stdscr, rows, cols);
    game_name(rows, cols);
    Menu(rows, cols);


                       
    endwin();             
    return 0;
}