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


char **create_map(int width, int height, int level_difficulty, Player* player, Room *rooms, int num_rooms) {
    player->is_in_floor++;
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
    rooms[0].visited = 1;
    //تم بندی اتاق ها
    int num_regular_rooms = num_rooms / 2; 
    int num_treasure_rooms = 0, num_enchant_rooms = 1, num_nightmare_rooms = 1;
    if(player->is_in_floor == 5){
        num_treasure_rooms = 1;
    }
    while(num_treasure_rooms + num_enchant_rooms + num_nightmare_rooms + num_regular_rooms < num_rooms){
        num_enchant_rooms++;
    }
    for(int i = 0 ; i < num_regular_rooms ; i++){
        rooms[i].theme = 'r'; //regular rooms
    }
    if(player->is_in_floor == 5){
        rooms[num_regular_rooms].theme = 't'; //treasure room
    }
    for(int i = num_regular_rooms + num_treasure_rooms ; i < num_regular_rooms + 1 + num_enchant_rooms ; i++){
        rooms[i].theme = 'e'; //enchant rooms
    }
    rooms[num_regular_rooms + num_treasure_rooms + num_enchant_rooms].theme = 'n'; //nightmare rooms
    //گذاشتن چیز میزای دیگه تو اتاقا
    for(int i = 0; i < num_rooms ; i++){
        //پنجره داخل هر اتاق به طور تصادفی
        int num_window = rand() % 2;
        for(int j = 0 ; j < num_window ; j++){
            int win_x = rooms[i].start_x;
            int win_y = rooms[i].start_y + rand() % rooms[i].height;
            while(map[win_y][win_x] != '|'){
                win_y = rooms[i].start_y + rand() % rooms[i].height;
            }
            map[win_y][win_x] = '=';
        }
        //موانع داخل هر اتاق به طور تصادفی
        int num_obstacle = rooms[i].height/3 + rand() % 2;
        for(int j = 0 ; j < num_obstacle ; j++){
            int obs_x = rooms[i].start_x + rand() % (rooms[i].width);
            int obs_y = rooms[i].start_y + rand() % (rooms[i].height);
            while(map[obs_y][obs_x] != '.' 
                || (map[obs_y+1][obs_x] == '+') 
                || (map[obs_y-1][obs_x] == '+') 
                || (map[obs_y][obs_x+1] == '+') 
                || (map[obs_y][obs_x-1] == '+')){
                obs_x = rooms[i].start_x + rand() % (rooms[i].width);
                obs_y = rooms[i].start_y + rand() % (rooms[i].height);
            }
            map[obs_y][obs_x] = 'o';
        }
        int num_traps, num_talisman, num_gold;
        if(rooms[i].theme == 'r'){
            num_gold = 2;
            num_talisman = rand() % 2;
            num_traps = rand() % 2;
        } else if(rooms[i].theme == 't'){
            num_gold = 3;      
            num_talisman = 0;             
            num_traps = rooms[i].width/3 + rand() % 3;           
        }  else if(rooms[i].theme == 'n'){
            int num_gold_halluc = rand() % 4; 
            num_gold = 0;
            num_talisman= rand() % 2;
            num_traps = 0;
            //گذاشتن طلا در اتاق
            for(int j = 0 ; j < num_gold_halluc ; j++){
                int gold_x = rooms[i].start_x + rand() % (rooms[i].width);
                int gold_y = rooms[i].start_y + rand() % (rooms[i].height);
                while(map[gold_y][gold_x] != '.'){
                    gold_x = rooms[i].start_x + rand() % (rooms[i].width);
                    gold_y = rooms[i].start_y + rand() % (rooms[i].height);
                }
                map[gold_y][gold_x] = 'h';
            }
        } else if(rooms[i].theme == 'e'){
            num_gold =  rand() % 2;
            num_talisman = rand() % 2;
            num_traps =  rand() % 2;
            //مخفی کردن در های اتاق
            for (int y = rooms[i].start_y; y < rooms[i].start_y + rooms[i].height; y++) {
                for (int x = rooms[i].start_x; x < rooms[i].start_x + rooms[i].width; x++) {
                    if (map[y][x] == '+') {
                        map[y][x] == 'v';
                    }
                }
            }
        }
        
        //گذاشتن طلا در اتاق
        for(int j = 0 ; j < num_gold ; j++){
            int gold_x = rooms[i].start_x + rand() % (rooms[i].width);
            int gold_y = rooms[i].start_y + rand() % (rooms[i].height);
            while(map[gold_y][gold_x] != '.'){
                gold_x = rooms[i].start_x + rand() % (rooms[i].width);
                gold_y = rooms[i].start_y + rand() % (rooms[i].height);
            }
            map[gold_y][gold_x] = 'g';
        }
        ///گذاشتن طلسم در اتاق
        for(int j = 0 ; j < num_talisman ; j++){
            int talism_x = rooms[i].start_x + rand() % (rooms[i].width);
            int talism_y = rooms[i].start_y + rand() % (rooms[i].height);
            while(map[talism_y][talism_x] != '.'){
                talism_x = rooms[i].start_x + rand() % (rooms[i].width);
                talism_y = rooms[i].start_y + rand() % (rooms[i].height);
            }
            map[talism_y][talism_x] = 't';
        }
        //گذاشتن تله در اتاق
        for(int j = 0 ; j < num_traps ; j++){
            int trap_x = rooms[i].start_x + rand() % (rooms[i].width);
            int trap_y = rooms[i].start_y + rand() % (rooms[i].height);
            while(map[trap_y][trap_x] != '.'){
                trap_x = rooms[i].start_x + rand() % (rooms[i].width);
                trap_y = rooms[i].start_y + rand() % (rooms[i].height);
            }
            map[trap_y][trap_x] = 'y';
        }
    }


    return map;
}

void display_map(char **map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited) {
    start_color();
    init_pair(140,140,COLOR_BLACK);
    init_pair(178, 178, COLOR_BLACK);
    init_pair(25, 25, COLOR_BLACK);
    init_pair(95, 95, COLOR_BLACK);
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    const char *emojis[] = {"☠️", "☁️", "☺️", "\u2695", "\u26c0", "\u25B2"};
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
            if(rooms[r].theme == 'r'){
                attron(COLOR_PAIR(140));
            } else if(rooms[r].theme == 't'){
                attron(COLOR_PAIR(178));
            } else if(rooms[r].theme == 'e'){
                attron(COLOR_PAIR(25));
            } else if(rooms[r].theme == 'n'){
                attron(COLOR_PAIR(95));
            }
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
                        if(rooms[r].theme == 'r'){
                            attron(COLOR_PAIR(140));
                        } else if(rooms[r].theme == 't'){
                            attron(COLOR_PAIR(178));
                        } else if(rooms[r].theme == 'e'){
                            attron(COLOR_PAIR(25));
                        } else if(rooms[r].theme == 'n'){
                            attron(COLOR_PAIR(95));
                        }
                    } else if (map[y][x] == '=') { // پنجره اتاق
                        mvprintw(y, x, "=");
                    }else if(map[y][x] == 'g'){
                        if(map_visited[y][x] == 0){
                            mvprintw(y, x, "\u26c0");
                        } else if(map_visited[y][x] == 1){
                            mvprintw(y, x, ".");
                        }
                    } else if(map[y][x] == 't'){
                        mvprintw(y, x, "\u2695");
                    } else if(map[y][x] == 'y'){
                        if(map_visited[y][x] == 0){
                            mvprintw(y, x, "\u25B2");
                        } else if(map_visited[y][x] == 1){
                            mvprintw(y, x, ".");
                        }
                    } else if(map[y][x] == 'h'){
                        if(map_visited[y][x] == 0){
                            mvprintw(y, x, "\u26c0");
                        } else if(map_visited[y][x] == 1){
                            mvprintw(y, x, ".");
                        }
                    } else if(map[y][x] == 'v'){
                        mvprintw(y, x, " ");
                    }
                }
            }
            if(rooms[r].theme == 'r'){
                attroff(COLOR_PAIR(140));
            } else if(rooms[r].theme == 't'){
                attroff(COLOR_PAIR(178));
            } else if(rooms[r].theme == 'e'){
                attroff(COLOR_PAIR(25));
            } else if(rooms[r].theme == 'n'){
                attroff(COLOR_PAIR(95));
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
            if (map_visited[j][i] == 1 && (map[j][i] == '#')){
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

void display_whole_map(char ** map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited){
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(140,140,COLOR_BLACK);
    init_pair(178, 178, COLOR_BLACK);
    init_pair(25, 25, COLOR_BLACK);
    init_pair(95, 95, COLOR_BLACK);
    const char *emojis[] = {"☠️", "☁️", "☺️", "\u2695", "\u26c0", "\u25B2"};

    for (int r = 0; r < num_rooms; r++) {
        if(rooms[r].theme == 'r'){
            attron(COLOR_PAIR(140));
        } else if(rooms[r].theme == 't'){
            attron(COLOR_PAIR(178));
        } else if(rooms[r].theme == 'e'){
            attron(COLOR_PAIR(25));
        } else if(rooms[r].theme == 'n'){
            attron(COLOR_PAIR(95));
        }
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
                    if(rooms[r].theme == 'r'){
                        attron(COLOR_PAIR(140));
                    } else if(rooms[r].theme == 't'){
                        attron(COLOR_PAIR(178));
                    } else if(rooms[r].theme == 'e'){
                        attron(COLOR_PAIR(25));
                    } else if(rooms[r].theme == 'n'){
                        attron(COLOR_PAIR(95));
                    }
                } else if (map[y][x] == '=') { // در اتاق
                    mvprintw(y, x, "=");
                } 
            }
        }
        if(rooms[r].theme == 'r'){
            attroff(COLOR_PAIR(140));
        } else if(rooms[r].theme == 't'){
            attroff(COLOR_PAIR(178));
        } else if(rooms[r].theme == 'e'){
            attroff(COLOR_PAIR(25));
        } else if(rooms[r].theme == 'n'){
            attroff(COLOR_PAIR(95));
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
            if (map[j][i] == '#'){
                mvprintw(j, i, "%c", map[j][i]);
            } else if(map[j][i] == 'g'){
                if(map_visited[j][i] == 0){
                    mvprintw(j, i, "\u26c0");
                } else if(map_visited[j][i] == 1){
                    mvprintw(j, i, ".");
                }
            } else if(map[j][i] == 't'){
                mvprintw(j, i, "\u2695");
            } else if(map[j][i] == 'y'){
                if(map_visited[j][i] == 0){
                    mvprintw(j, i, "\u25B2");
                } else if(map_visited[j][i] == 1){
                    mvprintw(j, i, ".");
                }
            } else if(map[j][i] == 'h'){
                if(map_visited[j][i] == 0){
                    mvprintw(j, i, "\u26c0");
                } else if(map_visited[j][i] == 1){
                    mvprintw(j, i, ".");
                }
            } else if(map[j][i] == 'v'){
                mvprintw(j, i, " ");
            }
        }
    }
    
}