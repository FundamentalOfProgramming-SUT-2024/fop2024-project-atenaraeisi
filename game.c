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

void show_message(WINDOW *msg_win, const char *message) {
    wclear(msg_win); // پاک کردن محتوای قبلی پنجره
    box(msg_win, 0, 0); // افزودن کادر دور پنجره
    mvwprintw(msg_win, 1, 1, "%s", message); // نوشتن پیام جدید
    wrefresh(msg_win); // به‌روزرسانی پنجره
}

/*
        // ایجاد پنجره برای پیام‌ها
        WINDOW *msg_win = newwin(3, 50, 1, 1);
        show_message(msg_win, "Game saved successfully!");
        wrefresh(msg_win);
        // مکث برای مشاهده پیام
        sleep(2);
*/

//int height = 3, width = 50, start_y = 1, start_x = 1;
    // ایجاد پنجره برای پیام‌ها
//WINDOW *msg_win = newwin(height, width, start_y, start_x);
//wrefresh(msg_win);

void save_game_to_binary_file(char **map, int rows, int cols, Room *rooms, int num_rooms, Player *player,  int **map_visited) {
    FILE *file = fopen("previous_game.bin", "wb");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // ذخیره تعداد سطرها و ستون‌ها
    fwrite(&rows, sizeof(int), 1, file);
    fwrite(&cols, sizeof(int), 1, file);

    // ذخیره نقشه
    for (int i = 0; i < rows; i++) {
        fwrite(map[i], sizeof(char), cols, file);
    }

    // ذخیره map_visited
    for (int i = 0; i < rows; i++) {
        fwrite(map_visited[i], sizeof(int), cols, file);
    }


    // ذخیره تعداد اتاق‌ها
    fwrite(&num_rooms, sizeof(int), 1, file);

    // ذخیره اطلاعات اتاق‌ها
    fwrite(rooms, sizeof(Room), num_rooms, file);

    // ذخیره اطلاعات بازیکن
    fwrite(player, sizeof(Player), 1, file);

    fclose(file);
}

void load_game_from_binary_file(char ***map, int *rows, int *cols, Room **rooms, int *num_rooms, Player *player, int ***map_visited) {
    FILE *file = fopen("previous_game.bin", "rb");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // خواندن تعداد سطرها و ستون‌ها
    fread(rows, sizeof(int), 1, file);
    fread(cols, sizeof(int), 1, file);

    // تخصیص حافظه برای نقشه
    *map = (char **)malloc(*rows * sizeof(char *));
    for (int i = 0; i < *rows; i++) {
        (*map)[i] = (char *)malloc(*cols * sizeof(char));
        fread((*map)[i], sizeof(char), *cols, file);
    }

    *map_visited = (int **)malloc(*rows * sizeof(int *));
    for (int i = 0; i < *rows; i++) {
        (*map_visited)[i] = (int *)malloc(*cols * sizeof(int));
    }
    // خواندن `map_visited`
    for (int i = 0; i < *rows; i++) {
        fread((*map_visited)[i], sizeof(int), *cols, file);
    }

    

    // خواندن تعداد اتاق‌ها
    fread(num_rooms, sizeof(int), 1, file);

    // تخصیص حافظه برای اتاق‌ها
    *rooms = (Room *)malloc(*num_rooms * sizeof(Room));
    fread(*rooms, sizeof(Room), *num_rooms, file);

    // خواندن اطلاعات بازیکن
    fread(player, sizeof(Player), 1, file);

    fclose(file);
}

void show_profile(){

}

void continue_game() {
    // تعریف متغیرها برای نقشه بازی
    setlocale(LC_ALL, "");
    int width, height, num_rooms;    
    getmaxyx(stdscr, height, width);
    height -= 5;

    char **map;
    int **map_visited;
    Room *rooms;
    Player player;

    // بارگذاری اطلاعات بازی
    load_game_from_binary_file(&map, &height, &width, &rooms, &num_rooms, &player, &map_visited);

    // تنظیم رنگ بازیکن
    player.color = hero_color;

    int display_completely = 0;
    // متغیر برای وضعیت بازی
    bool game_running = true;

    // حلقه اصلی بازی
    while (game_running) {
        // ذخیره وضعیت خانه قبلی
        char previous_cell = map[player.y][player.x];
        map_visited[player.y][player.x] = 1;

        // رسم بازیکن روی نقشه
        map[player.y][player.x] = '@';

        // نمایش نقشه
        clear();
        if(display_completely){
            display_whole_map(map, width, height, player, rooms, num_rooms);
        } else{
            display_map(map, width, height, player, rooms, num_rooms, map_visited);
        }        refresh();

        // دریافت ورودی از کاربر
        int ch = getch();

        // پاک کردن موقعیت قبلی بازیکن از نقشه
        map[player.y][player.x] = previous_cell; // بازگرداندن خانه قبلی به وضعیت قبلی

        // مدیریت ورودی‌ها
        switch (ch) {
            case KEY_UP:
                if (player.y > 0 && (map[player.y - 1][player.x] == '.' || map[player.y - 1][player.x] == '#' || map[player.y - 1][player.x] == '+')){
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                    
                break;
            case KEY_DOWN:
                if (player.y < height - 1 && (map[player.y + 1][player.x] == '.' || map[player.y + 1][player.x] == '#' || map[player.y + 1][player.x] == '+')){
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                    
                break;
            case KEY_LEFT:
                if (player.x > 0 && (map[player.y][player.x - 1] == '.' || map[player.y][player.x - 1] == '#' || map[player.y][player.x - 1] == '+')){
                    player.x--;
                    player.direction[0] = 'x';
                    player.direction[1] = '-';
                }
                   
                break;
            case KEY_RIGHT:
                if (player.x < width - 1 && (map[player.y][player.x + 1] == '.' || map[player.y][player.x + 1] == '#' || map[player.y][player.x + 1] == '+')){
                    player.x++;
                    player.direction[0] = 'x';
                    player.direction[1] = '+';
                }
                break;
            case KEY_PPAGE:
                if (player.y > 0 && player.x < width - 1 && (map[player.y-1][player.x + 1] == '.' || map[player.y-1][player.x + 1] == '#' || map[player.y-1][player.x + 1] == '+')){
                    player.x++;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_NPAGE:
                if (player.y < height - 1 && player.x < width - 1 && (map[player.y + 1][player.x + 1] == '.' || map[player.y + 1][player.x + 1] == '#' || map[player.y + 1][player.x + 1] == '+')){
                    player.x++;
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                break;
            case KEY_HOME:
                if (player.y > 0 && player.x > 0 && (map[player.y - 1][player.x - 1] == '.' || map[player.y - 1][player.x - 1] == '#' || map[player.y - 1][player.x - 1] == '+')){
                    player.x--;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_END:
                if (player.y < height - 1 && player.x > 0 && (map[player.y + 1][player.x - 1] == '.' || map[player.y + 1][player.x - 1] == '#' || map[player.y + 1][player.x - 1] == '+')){
                    player.x--;
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                break;
            case 'M': 
            case 'm':
                if(display_completely){
                    display_completely = 0;
                } else{
                    display_completely = 1;
                }
                break;
            case 'q': // خروج از بازی
                game_running = false; // پایان حلقه
                break;
            case 27: // خروج از بازی
                game_running = false; // پایان حلقه
                break;
        }
    }

    save_game_to_binary_file(map, height, width,rooms, num_rooms, &player, map_visited);

    // آزاد کردن حافظه
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
    free(rooms);
}

void new_game() {
    // تعریف متغیرها برای نقشه بازی
    setlocale(LC_ALL, "");
    int width, height;
    getmaxyx(stdscr, height, width);
    height -= 5;
    Player player;
    player.color = hero_color;

    
    int num_rooms;
    if(level_difficulty == 1){
        num_rooms = 6 + rand() % 2; // عداد اتاق‌ها بین 6 تا 7
    } else if(level_difficulty == 2){
        num_rooms = 7 + rand() % 2; // عداد اتاق‌ها بین 7 تا 8
    } else if(level_difficulty == 3){
        num_rooms = 8 + rand() % 2; // عداد اتاق‌ها بین 8 تا 9
    }
    Room *rooms = (Room *)malloc(num_rooms * sizeof(Room));
    
    // ساخت نقشه بازی
    char **map = create_map(width, height, level_difficulty, rooms, num_rooms);

    //مکان هایی از نقشه که دیده شدند
    int **map_visited = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++) {
        map_visited[i] = (int *)malloc(width * sizeof(int));
    }
    for (int i = 0; i < width ; i++){
        for(int j = 0; j < height; j++){
            map_visited[j][i] = 0;
        }
    }
    //اتاق اول دیده شه
    rooms[0].visited = 1;

    // افزودن بازیکن در موقعیت تصادفی
    player.x = rooms[0].start_x + rooms[0].width / 2 ;
    player.y = rooms[0].start_y + rooms[0].height  -1 - rand() % 5;
    while (!is_valid_position(map, player.x, player.y)){ // پیدا کردن موقعیت خالی
        player.y = rooms[0].start_y + rooms[0].height  -1 - rand() % 5;
    }

    int display_completely = 0;
    bool game_running = true;

    // حلقه اصلی بازی
    while (game_running) {
        // ذخیره وضعیت خانه قبلی
        char previous_cell = map[player.y][player.x];
        map_visited[player.y][player.x] = 1;

        // رسم بازیکن روی نقشه
        map[player.y][player.x] = '@';

        // نمایش نقشه
        clear();
        if(display_completely){
            display_whole_map(map, width, height, player, rooms, num_rooms);
        } else{
            display_map(map, width, height, player, rooms, num_rooms, map_visited);
        }
        refresh();

        // دریافت ورودی از کاربر
        int ch = getch();

        // پاک کردن موقعیت قبلی بازیکن از نقشه
        map[player.y][player.x] = previous_cell; // بازگرداندن خانه قبلی به وضعیت قبلی

        // مدیریت ورودی‌ها
        switch (ch) {
            case KEY_UP:
                if (player.y > 0 && (map[player.y - 1][player.x] == '.' || map[player.y - 1][player.x] == '#' || map[player.y - 1][player.x] == '+')){
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                    
                break;
            case KEY_DOWN:
                if (player.y < height - 1 && (map[player.y + 1][player.x] == '.' || map[player.y + 1][player.x] == '#' || map[player.y + 1][player.x] == '+')){
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                    
                break;
            case KEY_LEFT:
                if (player.x > 0 && (map[player.y][player.x - 1] == '.' || map[player.y][player.x - 1] == '#' || map[player.y][player.x - 1] == '+')){
                    player.x--;
                    player.direction[0] = 'x';
                    player.direction[1] = '-';
                }
                   
                break;
            case KEY_RIGHT:
                if (player.x < width - 1 && (map[player.y][player.x + 1] == '.' || map[player.y][player.x + 1] == '#' || map[player.y][player.x + 1] == '+')){
                    player.x++;
                    player.direction[0] = 'x';
                    player.direction[1] = '+';
                }
                break;
            case KEY_PPAGE:
                if (player.y > 0 && player.x < width - 1 && (map[player.y-1][player.x + 1] == '.' || map[player.y-1][player.x + 1] == '#' || map[player.y-1][player.x + 1] == '+')){
                    player.x++;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_NPAGE:
                if (player.y < height - 1 && player.x < width - 1 && (map[player.y + 1][player.x + 1] == '.' || map[player.y + 1][player.x + 1] == '#' || map[player.y + 1][player.x + 1] == '+')){
                    player.x++;
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                break;
            case KEY_HOME:
                if (player.y > 0 && player.x > 0 && (map[player.y - 1][player.x - 1] == '.' || map[player.y - 1][player.x - 1] == '#' || map[player.y - 1][player.x - 1] == '+')){
                    player.x--;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_END:
                if (player.y < height - 1 && player.x > 0 && (map[player.y + 1][player.x - 1] == '.' || map[player.y + 1][player.x - 1] == '#' || map[player.y + 1][player.x - 1] == '+')){
                    player.x--;
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                break;
            case 'M': 
            case 'm':
                if(display_completely){
                    display_completely = 0;
                } else{
                    display_completely = 1;
                }
                break;
            case 'q': // خروج از بازی
                game_running = false; // پایان حلقه
                break;
            case 27: // خروج از بازی
                game_running = false; // پایان حلقه
                break;
        }
    }

    save_game_to_binary_file(map, height, width,rooms, num_rooms, &player, map_visited);


    // آزاد کردن حافظه نقشه
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}


//......................................................................

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

        mvprintw(row - 2, 0, "Use UP/DOWN arrows to scroll. Press 'q' to quit.");

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
