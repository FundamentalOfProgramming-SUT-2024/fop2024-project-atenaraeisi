#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include "game.h"
#include "startup.h"
#include "create_map.h"

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
void continue_game(){

}
void Scoreboard(){

}


void new_game(char hero_color, int level_difficulty) {
    // تعریف متغیرها برای نقشه بازی
    setlocale(LC_ALL, "");
    int width, height;
    getmaxyx(stdscr, height, width);

    // ساخت نقشه بازی
    char **map = create_map(width, height, level_difficulty);

    
    

    // نمایش نقشه بازی
    display_map(map, width, height, hero_color);
    // نمایش
    refresh();


    // // حلقه اصلی بازی
    // while (true) {
    //     int ch = getch();  // دریافت ورودی از کاربر
    //     handle_input(ch);

    //     // منطق بازی رو اینجا قرار بدید

    //     // به روز رسانی نمایش نقشه
    //     display_map(map, width, height);
    //     // نمایش
    //     refresh();

    // }
    getch();

    // آزاد کردن حافظه نقشه
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}
