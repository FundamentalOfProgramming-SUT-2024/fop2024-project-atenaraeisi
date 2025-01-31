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
#define HUNGER_DECREASE_INTERVAL 5 // کاهش گرسنگی هر 5 ثانیه

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
    //------------------
    int height = 3, width = 50, start_y = 1, start_x = 1;
    //ایجاد پنجره برای پیام‌ها
    WINDOW *msg_win = newwin(height, width, start_y, start_x);
    wrefresh(msg_win);
*/


void update_hunger(Player *player) {
    if (player->hunger > 0) {
        player->hunger -= 1; // کاهش گرسنگی
    } else if (player->health > 0) {
        player->health -= 1; // کاهش سلامتی به دلیل گرسنگی شدید
    }
}
void display_hunger_bar_ncurses(int hunger, int max_hunger, int y, int x) {
    int bar_length = 20; // طول نوار گرسنگی
    int filled = (hunger * bar_length) / max_hunger;

    mvprintw(y, x, "Hunger: [");
    for (int i = 0; i < filled; i++) {
        printw("#"); // قسمت پر شده
    }
    for (int i = filled; i < bar_length; i++) {
        printw("-"); // قسمت خالی
    }
    printw("] %d%%", hunger);

    refresh(); // به‌روزرسانی پنجره
}
void display_health_bar_ncurses(int health, int max_health, int y, int x) {
    int bar_length = 20; // طول نوار گرسنگی
    int filled = (health * bar_length) / max_health;

    mvprintw(y, x, "Health: [");
    for (int i = 0; i < filled; i++) {
        printw("#"); // قسمت پر شده
    }
    for (int i = filled; i < bar_length; i++) {
        printw("-"); // قسمت خالی
    }
    printw("] %d%%", health, max_health);

    refresh(); // به‌روزرسانی پنجره
}
void consume_food_ncurses(Player *player, WINDOW *menu_win) {
    if (player->food_count == 0) {
        mvwprintw(menu_win, 1, 1, "No food available!");
        wrefresh(menu_win);
        sleep(1);
        return;
    }

    // نمایش منوی غذا
    mvwprintw(menu_win, 1, 1, "Choose a food to consume:");
    for (int i = 0; i < player->food_count; i++) {
        mvwprintw(menu_win, 2 + i, 1, "%d. %s (Restores %d hunger)", 
                  i + 1, player->inventory[i].name, player->inventory[i].restore_hunger);
    }
    mvwprintw(menu_win, 2 + player->food_count, 1, "Press the number of your choice: ");
    wrefresh(menu_win);

    // خواندن انتخاب کاربر
    int choice = wgetch(menu_win) - '0'; // تبدیل ورودی کاراکتری به عدد
    if (choice < 1 || choice > player->food_count) {
        mvwprintw(menu_win, 2 + player->food_count + 1, 1, "Invalid choice!");
        wrefresh(menu_win);
        sleep(1);
        return;
    }

    // مصرف غذا
    if(player->hunger == 100){
        mvwprintw(menu_win, 2 + player->food_count + 2, 1, "You are full!");
        wrefresh(menu_win);
        sleep(1);
    } else{
        Food selected_food = player->inventory[choice - 1];
        player->hunger += selected_food.restore_hunger;
        if (player->hunger > 100) {
            player->hunger = 100;
        }

        // حذف غذا از موجودی
        for (int i = choice - 1; i < player->food_count - 1; i++) {
            player->inventory[i] = player->inventory[i + 1];
        }
        player->food_count--; 
        mvwprintw(menu_win, 2 + player->food_count + 2, 1, "You consumed %s.", selected_food.name);
        wrefresh(menu_win);               
        sleep(1);
    }
}
int can_go(int y, int x, char **map, Player* player, int ***map_visited, int g_clicked, Food foods[7]){
    if(map[y][x] == '.' || map[y][x] == '#' || map[y][x] == '+' || map[y][x] == 'h'){
        *map_visited[y][x] == 1;
        return 1;
    } else if(map[y][x] == 'y'){
        player->lives--;
        return 1;
    } else if(map[y][x] == 'g' && g_clicked == 0){
        player->collected_golds++;
        return 1;
    } else if(map[y][x] == '<'){
        player->is_in_floor++;
        return 1;
    } else if(map[y][x] == 'v'){
        return 1;
    } else if(map[y][x] == 'f'){
        Food food;
        for(int i = 0; i < 7; i++){
            if(foods[i].x = x && foods[i].y == y){
                food = foods[i];
            }
        }
        if (player->food_count < 5) {
            player->inventory[player->food_count++] = food;
            return 1;
        } else{
            WINDOW *msg_win = newwin(3, 50, 1, 1);
            show_message(msg_win, "You can't collect more than 5 foods");
            wrefresh(msg_win);
            // مکث برای مشاهده پیام
            sleep(1);
            return 0;
        }
    }
    else return 0;
}

void move_fast(char **map, int width, int height, Player *player, int **map_visited, Room *rooms, int num_rooms, int display_completely, Food foods[7]) {
    // نمایش اولیه برای منتظر بودن جهت حرکت
    mvprintw(height + 3, width/2 + 2, "Press a direction key (arrow keys) after 'f' to move fast...");
    refresh();

    // دریافت کلید جهت پس از زدن 'f'
    int direction_key = getch();

    // تعیین جهت حرکت بر اساس کلید جهت
    int dx = 0, dy = 0;
    switch (direction_key) {
        case KEY_UP:
            dy = -1;
            break;
        case KEY_DOWN:
            dy = 1;
            break;
        case KEY_LEFT:
            dx = -1;
            break;
        case KEY_RIGHT:
            dx = 1;
            break;
        default:
            return; // اگر کلید معتبر نباشد، تابع را ترک می‌کنیم
    }

    // حرکت سریع در جهت مشخص‌شده
    while (1) {
        int new_y = player->y + dy;
        int new_x = player->x + dx;

        // بررسی محدودیت‌های نقشه و مانع‌ها
        if (new_y < 0 || new_y >= height || new_x < 0 || new_x >= width || !can_go(new_y, new_x, map, player, &map_visited, 0, foods)) {
            break;
        }

        player->y = new_y;
        player->x = new_x;

        // توقف در صورت برخورد به شیء خاص
        if (map[new_y][new_x] == 'g' || map[new_y][new_x] == 'h') {
            break;
        }
    }
}

void new_game() {
    // تعریف متغیرها برای نقشه بازی
    setlocale(LC_ALL, "");
    int width, height;
    getmaxyx(stdscr, height, width);
    int whole_height = height;
    height -= 5;
    Player player;
    player.color = hero_color;
    player.is_in_floor = 1;
    player.collected_golds = 0;
    player.points = 0;
    player.lives = 5;
    player.health = 100;
    player.food_count = 0;
    player.hunger = 70;
    // پنجره منوی غذا
    WINDOW *menu_win = newwin(10, 40, 5, 5);
    
    int num_rooms;
    if(level_difficulty == 1){
        num_rooms = 6 + rand() % 2; // عداد اتاق‌ها بین 6 تا 7
    } else if(level_difficulty == 2){
        num_rooms = 7 + rand() % 2; // عداد اتاق‌ها بین 7 تا 8
    } else if(level_difficulty == 3){
        num_rooms = 8 + rand() % 2; // عداد اتاق‌ها بین 8 تا 9
    }
    Room *rooms = (Room *)malloc(num_rooms * sizeof(Room));
    Food foods[7];
    
    // ساخت نقشه بازی
    char **map = create_map(width, height, level_difficulty, &player, rooms, num_rooms, foods);

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
    int g_clicked = 0;
    time_t last_update = time(NULL);    
    // حلقه اصلی بازی
    while (game_running) {
        // بررسی زمان برای کاهش گرسنگی
        if (difftime(time(NULL), last_update) >= HUNGER_DECREASE_INTERVAL) {
            update_hunger(&player);
            last_update = time(NULL);
        }

        // بررسی پایان بازی
        if (player.health <= 0) {
            WINDOW *msg_win = newwin(3, 35, height/2, (width - 35)/2);
            show_message(msg_win, "Game Over! You starved to death.");
            wrefresh(msg_win);
            // مکث برای مشاهده پیام
            sleep(2);
            break;
        }

        // ذخیره وضعیت خانه قبلی
        char previous_cell = map[player.y][player.x];
        if(!g_clicked){
            map_visited[player.y][player.x] = 1;   
        }

        if(previous_cell == '<'){
            map = create_map(width, height, level_difficulty, &player, rooms, num_rooms, foods);
            previous_cell = '.';
        }
        // رسم بازیکن روی نقشه
        map[player.y][player.x] = '@';
        // نمایش نقشه
        clear();
        if(display_completely){
            display_whole_map(map, width, height, player, rooms, num_rooms, map_visited);
        } else{
            display_map(map, width, height, player, rooms, num_rooms, map_visited);
        }
        for (int i = 0; i < width; i++) {
            mvaddch(whole_height - 5, i, '-'); // در ردیف 0، کاراکتر "-" را رسم می‌کنیم
        }
        for (int i = 0; i < 5; i++) {
            mvaddch(whole_height - i, width/2, '|'); // در ردیف 0، کاراکتر "-" را رسم می‌کنیم
        }
        attron(A_BOLD);
        mvprintw(whole_height - 4, 5, "Lives: %d", player.lives);
        mvprintw(whole_height - 3, 5, "Floor: %d", player.is_in_floor);
        mvprintw(whole_height - 2, 5, "Golds: %d", player.collected_golds);
        mvprintw(whole_height - 1, 5, "Points: %d", player.points);
        //mvprintw(whole_height - 4, width/6, "Health: %d%%", player.health);
        display_health_bar_ncurses(player.health, 100, whole_height - 4 , width/6);
        // mvprintw(whole_height - 2, width/6, "hunger: %d%%", player.hunger);
        display_hunger_bar_ncurses(player.hunger, 100, whole_height - 2 , width/6);        
        attroff(A_BOLD);
        mvprintw(whole_height - 4, width/2 + 2, "Press q/Esc to exit the game (note:game will be saved).");        
        refresh();
        // دریافت ورودی از کاربر
        int ch = getch();

        // پاک کردن موقعیت قبلی بازیکن از نقشه
        map[player.y][player.x] = previous_cell; // بازگرداندن خانه قبلی به وضعیت قبلی
        if(previous_cell == 'g' && g_clicked == 0){
            WINDOW *msg_win = newwin(3, 25, height/2, (width - 25)/2);
            show_message(msg_win, "You collected 1 gold!");
            wrefresh(msg_win);
            // مکث برای مشاهده پیام
            sleep(1);
        }
        g_clicked = 0;
        // مدیریت ورودی‌ها
        switch (ch) {
            case KEY_UP:
                if (player.y > 0 && can_go(player.y-1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                    
                break;
            case KEY_DOWN:
                if (player.y < height - 1 && can_go(player.y + 1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                    
                break;
            case KEY_LEFT:
                if (player.x > 0 && can_go(player.y , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x--;
                    player.direction[0] = 'x';
                    player.direction[1] = '-';
                }
                   
                break;
            case KEY_RIGHT:
                if (player.x < width - 1 && can_go(player.y , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x++;
                    player.direction[0] = 'x';
                    player.direction[1] = '+';
                }
                break;
            case KEY_PPAGE:
                if (player.y > 0 && player.x < width - 1 && can_go(player.y-1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x++;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_NPAGE:
                if (player.y < height - 1 && player.x < width - 1 && can_go(player.y + 1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x++;
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                break;
            case KEY_HOME:
                if (player.y > 0 && player.x > 0 && can_go(player.y-1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x--;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_END:
                if (player.y < height - 1 && player.x > 0 && can_go(player.y + 1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
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
            case 'F':
            case 'f': // حرکت سریع پس از دریافت جهت
                move_fast(map, width, height, &player, map_visited, rooms, num_rooms, display_completely, foods);
                break;
            case 'G':
            case 'g':
                g_clicked = 1;
                int ch2 = getch();
                switch (ch2) {
                    case KEY_UP:
                        if (player.y > 0 && can_go(player.y-1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                            
                        break;
                    case KEY_DOWN:
                        if (player.y < height - 1 && can_go(player.y + 1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                            
                        break;
                    case KEY_LEFT:
                        if (player.x > 0 && can_go(player.y , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.direction[0] = 'x';
                            player.direction[1] = '-';
                        }
                        
                        break;
                    case KEY_RIGHT:
                        if (player.x < width - 1 && can_go(player.y , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.direction[0] = 'x';
                            player.direction[1] = '+';
                        }
                        break;
                    case KEY_PPAGE:
                        if (player.y > 0 && player.x < width - 1 && can_go(player.y-1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                        break;
                    case KEY_NPAGE:
                        if (player.y < height - 1 && player.x < width - 1 && can_go(player.y + 1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                        break;
                    case KEY_HOME:
                        if (player.y > 0 && player.x > 0 && can_go(player.y-1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                        break;
                    case KEY_END:
                        if (player.y < height - 1 && player.x > 0 && can_go(player.y + 1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                        break;
                }
                break;
            case 'E':
            case 'e':
                werase(menu_win); // پاک کردن محتوای قبلی
                box(menu_win, 0, 0);
                consume_food_ncurses(&player, menu_win);
                break;
            case 'q': // خروج از بازی
                game_running = false; // پایان حلقه
                break;
            case 27: // خروج از بازی
                game_running = false; // پایان حلقه
                break;
        }
    }

    save_game_to_binary_file(map, height, width,rooms, num_rooms, &player, map_visited, foods);


    // آزاد کردن حافظه نقشه
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}

void continue_game() {
    // تعریف متغیرها برای نقشه بازی
    setlocale(LC_ALL, "");
    int width, height, num_rooms;    
    getmaxyx(stdscr, height, width);
    int whole_height = height;
    height -= 5;

    char **map;
    int **map_visited;
    Room *rooms;
    Player player;
    Food foods[7];

    // بارگذاری اطلاعات بازی
    load_game_from_binary_file(&map, &height, &width, &rooms, &num_rooms, &player, &map_visited, &foods);

    // تنظیم رنگ بازیکن
    player.color = hero_color;

    int display_completely = 0;
    // متغیر برای وضعیت بازی
    bool game_running = true;
    int g_clicked = 0;

    // حلقه اصلی بازی
    while (game_running) {
        // ذخیره وضعیت خانه قبلی
        char previous_cell = map[player.y][player.x];
        if(!g_clicked){
            map_visited[player.y][player.x] = 1;   
        }

        if(previous_cell == '<'){
            map = create_map(width, height, level_difficulty, &player, rooms, num_rooms, foods);
            previous_cell = '.';
        }
        // رسم بازیکن روی نقشه
        map[player.y][player.x] = '@';

        // نمایش نقشه
        clear();
        if(display_completely){
            display_whole_map(map, width, height, player, rooms, num_rooms, map_visited);
        } else{
            display_map(map, width, height, player, rooms, num_rooms, map_visited);
        }
        for (int i = 0; i < width; i++) {
            mvaddch(whole_height - 5, i, '-'); // در ردیف 0، کاراکتر "-" را رسم می‌کنیم
        }
        for (int i = 0; i < 5; i++) {
            mvaddch(whole_height - i, width/2, '|'); // در ردیف 0، کاراکتر "-" را رسم می‌کنیم
        }
        attron(A_BOLD);
        mvprintw(whole_height - 4, 5, "Lives: %d", player.lives);
        mvprintw(whole_height - 2, 5, "Health: %d%%", player.health);
        mvprintw(whole_height - 4, width/4, "Golds: %d", player.collected_golds);
        mvprintw(whole_height - 2, width/4, "Points: %d", player.points);
        mvprintw(whole_height - 4, width/2 - 11, "Floor: %d", player.is_in_floor);
        mvprintw(whole_height - 2, width/2 - 11, "hunger: %d%%", player.hunger);        
        attroff(A_BOLD);
        mvprintw(whole_height - 4, width/2 + 2, "Press q/Esc to exit the game (note:game will be saved).");        
        refresh();

        // دریافت ورودی از کاربر
        int ch = getch();

        // پاک کردن موقعیت قبلی بازیکن از نقشه
        map[player.y][player.x] = previous_cell; // بازگرداندن خانه قبلی به وضعیت قبلی
        g_clicked = 0;
        // مدیریت ورودی‌ها
        switch (ch) {
            case KEY_UP:
                if (player.y > 0 && can_go(player.y-1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                    
                break;
            case KEY_DOWN:
                if (player.y < height - 1 && can_go(player.y + 1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                    player.y++;
                    player.direction[0] = 'y';
                    player.direction[1] = '+';
                }
                    
                break;
            case KEY_LEFT:
                if (player.x > 0 && can_go(player.y , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x--;
                    player.direction[0] = 'x';
                    player.direction[1] = '-';
                }
                   
                break;
            case KEY_RIGHT:
                if (player.x < width - 1 && can_go(player.y , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x++;
                    player.direction[0] = 'x';
                    player.direction[1] = '+';
                }
                break;
            case KEY_PPAGE:
                if (player.y > 0 && player.x < width - 1 && can_go(player.y-1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                    player.x++;
                    player.y--;
                    player.direction[0] = 'y';
                    player.direction[1] = '-';
                }
                break;
            case KEY_NPAGE:
                if (player.y < height - 1 && player.x < width - 1 && can_go(player.y + 1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
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
            case 'F':
            case 'f': // حرکت سریع پس از دریافت جهت
                move_fast(map, width, height, &player, map_visited, rooms, num_rooms, display_completely, foods);
                break;
            case 'G':
            case 'g':
                g_clicked = 1;
                int ch2 = getch();
                switch (ch2) {
                    case KEY_UP:
                        if (player.y > 0 && can_go(player.y-1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                            
                        break;
                    case KEY_DOWN:
                        if (player.y < height - 1 && can_go(player.y + 1 , player.x, map, &player, &map_visited, g_clicked, foods)){
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                            
                        break;
                    case KEY_LEFT:
                        if (player.x > 0 && can_go(player.y , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.direction[0] = 'x';
                            player.direction[1] = '-';
                        }
                        
                        break;
                    case KEY_RIGHT:
                        if (player.x < width - 1 && can_go(player.y , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.direction[0] = 'x';
                            player.direction[1] = '+';
                        }
                        break;
                    case KEY_PPAGE:
                        if (player.y > 0 && player.x < width - 1 && can_go(player.y-1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                        break;
                    case KEY_NPAGE:
                        if (player.y < height - 1 && player.x < width - 1 && can_go(player.y + 1 , player.x + 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x++;
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                        break;
                    case KEY_HOME:
                        if (player.y > 0 && player.x > 0 && can_go(player.y-1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.y--;
                            player.direction[0] = 'y';
                            player.direction[1] = '-';
                        }
                        break;
                    case KEY_END:
                        if (player.y < height - 1 && player.x > 0 && can_go(player.y + 1 , player.x - 1, map, &player, &map_visited, g_clicked, foods)){
                            player.x--;
                            player.y++;
                            player.direction[0] = 'y';
                            player.direction[1] = '+';
                        }
                        break;
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

    save_game_to_binary_file(map, height, width,rooms, num_rooms, &player, map_visited, foods);

    // آزاد کردن حافظه
    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
    free(rooms);
}