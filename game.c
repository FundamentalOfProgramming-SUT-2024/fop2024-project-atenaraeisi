#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>
#include "game.h"

void show_message(WINDOW *msg_win, const char *message) {
    wclear(msg_win); // پاک کردن محتوای قبلی پنجره
    mvwprintw(msg_win, 1, 1, "%s", message); // نوشتن پیام جدید
    wrefresh(msg_win); // به‌روزرسانی پنجره
}

//int height = 3, width = 50, start_y = 1, start_x = 1;
    // ایجاد پنجره برای پیام‌ها
//WINDOW *msg_win = newwin(height, width, start_y, start_x);
//wrefresh(msg_win);


void show_profile(){

}
void new_game(){
    //start_game();
}
void continue_game(){

}
void Scoreboard(){

}