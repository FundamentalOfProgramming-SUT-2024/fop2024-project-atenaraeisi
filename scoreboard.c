#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "game.h"
#include "startup.h"
#include "menu.h"
#include "create_map.h"

// تابع مقایسه برای مرتب‌سازی بر اساس points
int compare_users_by_points(const void *a, const void *b) {
    const user *userA = (const user *)a;
    const user *userB = (const user *)b;

    // ترتیب صعودی
    //return userA->points - userB->points;

    // برای ترتیب نزولی:
    return userB->points - userA->points;
}

// تابع مرتب‌سازی کاربران
void sort_users_by_points(user *users, int total_users) {
    qsort(users, total_users, sizeof(user), compare_users_by_points);
}


// تابع خواندن اطلاعات از فایل
int load_users_from_file(const char *filename, user **users) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int total_users = file_size / sizeof(user);
    *users = (user *)malloc(total_users * sizeof(user));
    if (!*users) {
        perror("Memory allocation error");
        fclose(file);
        return 0;
    }

    fread(*users, sizeof(user), total_users, file);
    fclose(file);

    return total_users;
}

// تابع نمایش جدول امتیازات
void display_leaderboard(user *users, int total_users, int logged_in_index) {
    initscr();
    start_color();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    // تعریف رنگ‌ها
    init_pair(1, COLOR_YELLOW, COLOR_BLACK); // رنگ برای سه کاربر برتر
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  // رنگ برای کاربران عادی
    init_pair(3, COLOR_GREEN, COLOR_BLACK);  // رنگ برای کاربر لاگین‌شده

    int row, col;
    getmaxyx(stdscr, row, col);
    int max_rows = row - 4; // تعداد سطرهای قابل نمایش

    int start_index = 0;
    int ch;

    while (1) {
        clear();
        mvprintw(0, (col - 15) / 2, "Leaderboard");
        mvprintw(2, 0, " Rank |       Username       | Points | Golds | Times of playing | Experience");
        mvprintw(3, 0, "-----------------------------------------------------------------------------");

        // نمایش کاربران از start_index
        for (int i = 0; i < max_rows && (start_index + i) < total_users; i++) {
            int idx = start_index + i;
            int color_pair = 2;

            // رنگ برای سه نفر برتر
            if (idx == 0) color_pair = 1;
            else if (idx == 1) color_pair = 1;
            else if (idx == 2) color_pair = 1;

            // رنگ برای کاربر لاگین‌شده
            if (idx == logged_in_index) color_pair = 3;
            attron(COLOR_PAIR(color_pair));
            if(user1 != NULL && strcmp(user1->UserName , users[idx].UserName) == 0){
                attron(A_BOLD);
            }
            
            mvprintw(4 + i, 0, " %4d | %-20s | %6d | %5d | %8d         | %4d     ",
                     idx + 1, users[idx].UserName, users[idx].points, users[idx].golds,
                     users[idx].times_played, users[idx].times_played);
            
            // مدال برای سه نفر برتر
            if (idx < 3) {
                mvprintw(4 + i, col - 6, idx == 0 ? "GOAT" : (idx == 1 ? "Legend" : "Pro"));
            }
            attroff(COLOR_PAIR(color_pair));            
            if(user1 != NULL && strcmp(user1->UserName , users[idx].UserName) == 0){
                attroff(A_BOLD);
            }

        }

        mvprintw(row - 2, col - strlen("Use UP/DOWN arrows to scroll. Press 'q' to quit."), "Use UP/DOWN arrows to scroll. Press 'q' to quit.");

        refresh();

        ch = getch();
        if (ch == 'q') break;
        else if (ch == KEY_DOWN && start_index + max_rows < total_users) start_index++;
        else if (ch == KEY_UP && start_index > 0) start_index--;
    }

    endwin();
}

void Scoreboard(){
    user *users = NULL;
    int total_users = load_users_from_file("users.bin", &users);

    if (total_users == 0) {
        // ایجاد پنجره برای پیام‌ها
        WINDOW *msg_win = newwin(3, 50, 1, 1);
        show_message(msg_win, "No users found in file.\n");
        wrefresh(msg_win);
        // مکث برای مشاهده پیام
        sleep(2);
        return;
    }

    int logged_in_index = 0; // فرض کنیم کاربر اول لاگین کرده است
    if (total_users > 0) {
        sort_users_by_points(users, total_users);
        display_leaderboard(users, total_users, logged_in_index);
    }
    
    free(users); // آزاد کردن حافظه
}
void show_profile(){

}
