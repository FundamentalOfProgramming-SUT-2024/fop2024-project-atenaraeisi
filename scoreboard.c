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

int compare_users_by_points(const void *a, const void *b) {
    const user *userA = (const user *)a;
    const user *userB = (const user *)b;

    if (userB->points != userA->points) {
        return userB->points - userA->points;  // ترتیب نزولی امتیاز
    } else {
        return userB->golds - userA->golds;    // ترتیب نزولی طلا در صورت تساوی امتیاز
    }
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

int merge_duplicate_users(user *users, int total_users) {
    for (int i = 0; i < total_users; i++) {
        for (int j = i + 1; j < total_users; j++) {
            if (strcmp(users[i].UserName, users[j].UserName) == 0) {
                // ادغام اطلاعات کاربران
                users[i].points += users[j].points;
                users[i].golds += users[j].golds;
                users[i].times_played += users[j].times_played;

                // حذف کاربر j از لیست
                for (int k = j; k < total_users - 1; k++) {
                    users[k] = users[k + 1];
                }
                total_users--;
                j--; // چون یکی از کاربران حذف شده، باید مجدداً همان اندیس را بررسی کنیم
            }
        }
    }
    return total_users;
}
int find_logged_in_index(user *users, int total_users, char *logged_in_username) {
    if(!have_account){
        return -1;
    }
    for (int i = 0; i < total_users; i++) {
        if (strcmp(users[i].UserName, logged_in_username) == 0) {
            return i;  // اندیس کاربر لاگین‌شده پیدا شد
        }
    }
    return 0;  // اگر کاربر پیدا نشد
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
        mvprintw(0, (col - 18) / 2, "🏆 Leaderboard 🏆");
        mvprintw(2, 0, " Rank |       Username       | Points | Golds | Times of playing | Experience");
        mvprintw(3, 0, "-----------------------------------------------------------------------------");

        // نمایش کاربران از start_index
        // نمایش کاربران از start_index
        for (int i = 0; i < max_rows && (start_index + i) < total_users; i++) {
            int idx = start_index + i;
            int color_pair = 2;  // پیش‌فرض برای کاربران عادی
            int text_attr = A_NORMAL;  // ویژگی پیش‌فرض برای متن

            // تغییر رنگ و ویژگی برای سه نفر برتر
            if (idx == 0) {
                init_pair(4, COLOR_BLACK, COLOR_YELLOW);  // پس‌زمینه زرد
                color_pair = 4;
            } else if (idx == 1) {
                init_pair(5, COLOR_BLACK, COLOR_CYAN);    // پس‌زمینه آبی روشن
                color_pair = 5;
            } else if (idx == 2) {
                init_pair(6, COLOR_BLACK, COLOR_MAGENTA); // پس‌زمینه صورتی
                color_pair = 6;
            }


            // رنگ و ویژگی برای کاربر لاگین‌شده
            if (have_account && idx == logged_in_index) {
                color_pair = 3;  // رنگ سبز برای کاربر لاگین‌شده
                text_attr = A_BLINK;  // چشمک‌زن به عنوان ویژگی اضافی برای کاربر لاگین‌شده
            }

            attron(COLOR_PAIR(color_pair));
            attron(text_attr);  // اعمال ویژگی‌های متنی

            if(idx == 0 || idx == 1 || idx == 2){
                mvprintw(4 + i, 0, " %4d | %-18s🥇 | %6d | %5d | %8d         | %4d     ",
                    idx + 1, users[idx].UserName, users[idx].points, users[idx].golds,
                    users[idx].times_played, users[idx].times_played);
            } else{
                mvprintw(4 + i, 0, " %4d | %-20s | %6d | %5d | %8d         | %4d     ",
                        idx + 1, users[idx].UserName, users[idx].points, users[idx].golds,
                        users[idx].times_played, users[idx].times_played);                
            }

            
            // مدال برای سه نفر برتر
            // مدال برای سه نفر برتر
            if (idx == 0) {
                mvprintw(4 + i, strlen("----------------------------------------------------------------------------"), "  ★ GOAT ★  ");
            } else if (idx == 1) {
                mvprintw(4 + i, strlen("----------------------------------------------------------------------------"), " ✪ Legend ✪ ");
            } else if (idx == 2) {
                mvprintw(4 + i, strlen("----------------------------------------------------------------------------"), "   ✰ Pro ✰  ");
            }


            attroff(COLOR_PAIR(color_pair));
            attroff(text_attr);  // حذف ویژگی‌های متنی
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

void Scoreboard() { 
    char *logged_in_username = NULL; 
    if(have_account){
        // تخصیص حافظه به اندازه نام کاربر
        logged_in_username = (char *)malloc(strlen(user1->UserName) + 1); 
        if (logged_in_username == NULL) {
            perror("Memory allocation error");
            return;
        }
        strcpy(logged_in_username, user1->UserName); 
    }
    user *users = NULL;
    int total_users = load_users_from_file("users.bin", &users);

    if (total_users == 0) {
        WINDOW *msg_win = newwin(3, 50, 1, 1);
        show_message(msg_win, "No users found in file.\n");
        wrefresh(msg_win);
        sleep(2);
        return;
    }

    // ادغام کاربران با نام یکسان
    total_users = merge_duplicate_users(users, total_users);

    // مرتب‌سازی بر اساس امتیاز و تعداد طلا
    sort_users_by_points(users, total_users);

    // پیدا کردن اندیس کاربر لاگین‌شده
    int logged_in_index = find_logged_in_index(users, total_users, logged_in_username);

    // نمایش جدول امتیازات
    display_leaderboard(users, total_users, logged_in_index);

    free(users);
}

void show_profile(){

}
