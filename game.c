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
        display_map(map, width, height, player, rooms, num_rooms, map_visited);
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
    user1->times_played++;

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
    
    int num_regular_rooms = num_rooms / 2; 

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
        display_map(map, width, height, player, rooms, num_rooms, map_visited);
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

// تابع برای خواندن تمام کاربران از فایل
void load_users() {
    FILE *file = fopen("users.bin", "rb"); // "rb" برای خواندن فایل باینری
    if (!file) {
        perror("Error opening file");
        return;
    }

    user temp_user;
    mvprintw(1,1, "Saved Users:\n");
    refresh();
    int i = 0;
    while (fread(&temp_user, sizeof(user), 1, file)) { // خواندن هر کاربر از فایل
        i+=7;
        mvprintw(2 + i, 1, "Username: %s\n", temp_user.UserName);
        mvprintw(3 + i, 1, "Email: %s\n", temp_user.email);
        mvprintw(4 + i, 1, "Rank: %d\n", temp_user.rank);
        mvprintw(5 + i, 1, "Points: %d\n", temp_user.points);
        mvprintw(6 + i, 1, "Golds: %d\n", temp_user.golds);
        mvprintw(7 + i, 1, "Times Played: %d\n", temp_user.times_played);
        mvprintw(8 + i, 1, "-----------------------------\n");
        refresh();
    }
    sleep(5);

    fclose(file);
}

void Scoreboard(){
    load_users();
}
