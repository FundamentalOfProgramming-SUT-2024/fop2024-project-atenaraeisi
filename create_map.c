#include <ncurses.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "create_map.h"
#include "game.h"


bool is_valid_position(char **map, int x, int y) {
    return map[y][x] == '.';
}
// bool is_valid_position(char **map, int x, int y) {
//     if(map[y][x] == '.' && map[y][x+1] == '.' && map[y+1][x] == '.' && map[y][x-1] == '.' && map[y-1][x] == '.'){
//         return true;
//     }
//     return false;
// }


void connect_rooms(char **map, int x1, int y1, int x2, int y2) {
    int x = x1;
    int y = y1;
    while (x != x2) {
        if((map[y][x] == '_' || map[y][x] == '|') && (map[y][x+1] == '_' || map[y][x-1] == '_' || map[y][x+1] == '|' || map[y][x-1] == '|')){
            map[y][x] = '+'; // در
            break;
        }
        if (map[y][x] == '|' || map[y][x] == '_') {
            map[y][x] = '+'; // در
        } else if (map[y][x] == ' ') {
            map[y][x] = '#'; // راهرو
        }
        x += (x < x2) ? 1 : -1; // حرکت به سمت x2
    }

    // حرکت در جهت عمودی
    while (y != y2) {
        if((map[y][x] == '_' || map[y][x] == '|') && (map[y+1][x] == '_' || map[y-1][x] == '_' || map[y+1][x] == '|' || map[y-1][x] == '|')){
            map[y][x] = '+'; // در
            break;
        }
        if (map[y][x] == '_' || map[y][x] == '|') {
            map[y][x] = '+'; // در
        } else if (map[y][x] == ' ') {
            map[y][x] = '#'; // راهرو
        }
        y += (y < y2) ? 1 : -1; // حرکت به سمت y2
    }
    while (x != x2) {
        if((map[y][x] == '_' || map[y][x] == '|') && (map[y][x+1] == '_' || map[y][x-1] == '_' || map[y][x+1] == '|' || map[y][x-1] == '|')){
            break;
        }
        if (map[y][x] == '|' || map[y][x] == '_') {
            map[y][x] = '+'; // در
        } else if (map[y][x] == ' ') {
            map[y][x] = '#'; // راهرو
        }
        x += (x < x2) ? 1 : -1; // حرکت به سمت x2
    }



}


char **create_map(int width, int height, int level_difficulty, Room *rooms, int num_rooms) {
    // تخصیص حافظه برای نقشه
    char **map = (char **)malloc(height * sizeof(char *));
    for (int i = 0; i < height; i++) {
        map[i] = (char *)malloc(width * sizeof(char));
        memset(map[i], ' ', width); // پر کردن نقشه با ' '
    }

    int r = 0;
    for (r = 0; r < num_rooms; r++) {
        Room room;
        room.width = (width / 10) + rand() % (width / 6 - width / 10 + 1);
        room.height = (height / 6) + rand() % (height / 6 - height / 10 + 1) + 1;
        room.start_x = rand() % (width - room.width); // موقعیت X شروع اتاق
        room.start_y = rand() % (height - room.height); // موقعیت Y شروع اتاق
        room.visited = 0;
        room.floor = 1;

        // بررسی تداخل
        bool overlap = false;
        for (int y = room.start_y; y < room.start_y + room.height && !overlap; y++) {
            for (int x = room.start_x; x < room.start_x + room.width; x++) {
                if (map[y][x] == '.' || map[y][x] == '|' || map[y][x] == '_') {
                    overlap = true;
                    break;
                }
            }
        }
        // اگر تداخل وجود ندارد، اتاق را رسم کنید
        if (!overlap) {
            for (int y = room.start_y; y < room.start_y + room.height; y++) {
                for (int x = room.start_x; x < room.start_x + room.width; x++) {
                    if((x == room.start_x && y!=room.start_y )|| (x == room.start_x + room.width-1  && y!=room.start_y)){
                        map[y][x] = '|';
                    }
                    else if(y == room.start_y|| y == room.start_y + room.height-1){
                        map[y][x] = '_';
                    }
                    else map[y][x] = '.'; // پر کردن خانه‌های اتاق
                }
            }
            rooms[r] = room; // ذخیره اطلاعات اتاق در آرایه
        }
        else{
            r--;
            continue;
        }
        
        //موانع داخل هر اتاق به طور تصادفی
        int num_obstacle = room.height/3 + rand() % 2;
        for(int j = 0 ; j < num_obstacle ; j++){
            int obs_x = room.start_x + room.width/3 + rand() % (room.width/3);
            int obs_y = room.start_y + room.height/3 + rand() % (room.height/3);
            while(map[obs_y][obs_x] != '.' 
                || (obs_y+1 > 0 && obs_y+1 < height && map[obs_y+1][obs_x] == '+') 
                || (obs_y-1 > 0 && obs_y-1 < height && map[obs_y-1][obs_x] == '+') 
                || (obs_x+1 > 0 && obs_x+1 < width && map[obs_y][obs_x+1] == '+') 
                || (obs_x+1 > 0 && obs_x-1 < width && map[obs_y][obs_x-1] == '+')){
                obs_x = room.start_x + room.width/3 + rand() % (room.width/3);
                obs_y = room.start_y + room.height/3 + rand() % (room.height/3);
            }
            map[obs_y][obs_x] = 'o';
        }
        
    }
    //وصل کردن اتاق ها با راهرو
    int prev_center_x = -1, prev_center_y = -1;
    for(int i = 0; i < num_rooms ; i++){
        //rooms[i]
        //مختصات مرکز اتاق
        int center_x = rooms[i].start_x + rooms[i].width / 2;
        int center_y = rooms[i].start_y + rooms[i].height / 2;

        // اگر اتاق قبلی وجود دارد، راهرو بین اتاق حاضر و قبلی رسم کن
        if (prev_center_x != -1 && prev_center_y != -1) {
            connect_rooms(map, prev_center_x, prev_center_y, center_x, center_y);
        }

        for (int j = 0; j < i; j++) { // اتصال اتاق جدید به اتاق‌های قبلی
            if (rand() % 6 == 0) { // به صورت تصادفی یکی از اتاق‌های قبلی را انتخاب کنید
                int prev_center_x2 = rooms[j].start_x + rooms[j].width / 2;
                int prev_center_y2 = rooms[j].start_y + rooms[j].height / 2;
                //if(abs(prev_center_x2-center_x) < width/3 && abs(prev_center_y2-center_y) < width/3)
                connect_rooms(map, prev_center_x2, prev_center_y2, center_x, center_y);
            }
        }
        // به‌روزرسانی مرکز اتاق قبلی
        prev_center_x = center_x;
        prev_center_y = center_y;
    }
    //پنجره داخل هر اتاق به طور تصادفی
    for(int i = 0; i < num_rooms ; i++){
        int num_window = rand() % 2;
        for(int j = 0 ; j < num_window ; j++){
            int win_x = rooms[i].start_x;
            int win_y = rooms[i].start_y + rand() % rooms[i].height;
            while(map[win_y][win_x] != '|'){
                win_y = rooms[i].start_y + rand() % rooms[i].height;
            }
            map[win_y][win_x] = '=';
        }
    }
    rooms[0].visited = 1;

    return map;
}

void display_map(char **map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited ) {
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    const char *emojis[] = {"☠️", "☁️", "☺️"};

    //مشخص کردن اتاق های دیده شده
    for (int r = 0; r < num_rooms; r++) {
        for (int y = rooms[r].start_y; y < rooms[r].start_y + rooms[r].height; y++) {
            for (int x = rooms[r].start_x; x < rooms[r].start_x + rooms[r].width; x++) {
                if(map_visited[y][x] == 1){
                    rooms[r].visited = 1;
                }
            }
        }
        
    }

    // حلقه برای نمایش اتاق‌ها که بازدید شده‌اند
    for (int r = 0; r < num_rooms; r++) {
        if (rooms[r].visited == 1) { // فقط اتاق‌های بازدید شده
            // نمایش دیوارهای اتاق
            for (int y = rooms[r].start_y; y < rooms[r].start_y + rooms[r].height; y++) {
                for (int x = rooms[r].start_x; x < rooms[r].start_x + rooms[r].width; x++) {
                    if (map[y][x] == '|' || map[y][x] == '_') { // دیوارهای اتاق
                        mvprintw(y, x, "%c", map[y][x]);
                    } else if (map[y][x] == '.') { // فضای خالی اتاق
                        mvprintw(y, x, ".");
                    } else if (map[y][x] == '+') { // در اتاق
                        mvprintw(y, x, "+");
                    } else if (map[y][x] == 'o') { // نمایش اشیاء خاص (مثلاً آیتم‌ها)
                        attron(COLOR_PAIR(1));
                        mvprintw(y, x, "o");
                        attroff(COLOR_PAIR(1));
                    } else if (map[y][x] == '=') { // در اتاق
                        mvprintw(y, x, "=");
                    } 
                }
            }
        }
    }

    // نمایش برای چیزهای دیگری که دیده شده و خود کاراکتر اصلی
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            if (map[j][i] == '@') { // نمایش بازیکن
                if (player.color == 'r') {
                    attron(COLOR_PAIR(2));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(2));
                } else if (player.color == 'g') {
                    attron(COLOR_PAIR(4));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(4));
                } else if (player.color == 'b') {
                    attron(COLOR_PAIR(3));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(3));
                }
            }
            if (map_visited[j][i] == 1 && map[j][i] == '#'){
                mvprintw(j, i, "%c", map[j][i]);
            }
        }
    }

    //نمایش 5 واحد جلوتر در راهرو
    if(map[player.y +1][player.x] == '#' || map[player.y][player.x - 1 ] == '#' || map[player.y][player.x + 1] == '#' || map[player.y -1][player.x] == '#'){
        if(player.direction[0] == 'x'){
            for(int a = 0; a < 5; a++){
                if(player.direction[1] == '+'){
                    if(player.x + a > 0 && player.x + a < width){
                        if(map[player.y][player.x + a] == '#'){
                            mvprintw(player.y, player.x + a, "#");
                        } 
                    }
                }
               else if(player.direction[1] == '-'){
                    if(player.x - a > 0 && player.x - a < width){
                        if(map[player.y][player.x - a] == '#'){
                            mvprintw(player.y, player.x - a, "#");
                        } 
                    }
               }
                    
            }
        }
        else if(player.direction[0] == 'y'){     
            if(player.direction[1] == '+'){
                for(int b = 0; b < 5; b++){
                    if(player.y + b < height && player.y + b > 0){
                        if(map[player.y + b][player.x] == '#'){
                            mvprintw(player.y + b, player.x, "#");
                        } 
                    }
                    
                }
            }
            else if(player.direction[1] == '-'){
                for(int b = 0; b < 5; b++){
                    if(player.y - b < height && player.y - b > 0){
                        if(map[player.y - b][player.x] == '#'){
                            mvprintw(player.y - b, player.x, "#");
                        } 
                    }
                    
                }
            }     
            
        }
        
    }
}

void display_whole_map(char ** map, int width, int height, Player player){
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    const char *emojis[] = {"☠️", "☁️", "☺️"};

    for(int i=0 ; i<width; i++){
        for(int j=0 ; j<height ; j++){
            if(map[j][i]=='@'){
                // int num_of_emoji = rand() % 3;
                // mvprintw(j, i, "%s", emojis[num_of_emoji]);
                if(player.color == 'r'){
                    attron(COLOR_PAIR(2));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(2));
                }else if(player.color == 'g'){
                    attron(COLOR_PAIR(4));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(4));
                } else if(player.color == 'b'){
                    attron(COLOR_PAIR(3));
                    mvprintw(j, i, "@");
                    attroff(COLOR_PAIR(3));
                }
            }
            else if(map[j][i]=='o'){
                attron(COLOR_PAIR(1));
                mvprintw(j, i, "o");
                attroff(COLOR_PAIR(1));
            }
            else mvprintw(j, i , "%c", map[j][i]);
        }
    }
    
}