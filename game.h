#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

typedef struct {
    int x;          // مختصات x غذا
    int y;          // مختصات y غذا
    int type;       // نوع غذا (۱: معمولی، ۲: اعلا، ۳: جادویی، ۴: فاسد)
} Food;

typedef struct{
    int x;
    int y;
    char color;
    char direction[2];
    int is_in_floor;
    int collected_golds;
    int points;
    int lives;
    int health;
    int hunger; // سطح گرسنگی
    Food inventory[5];
    int food_count;
}Player;

typedef struct {
    int start_x;      // مختصات شروع X
    int start_y;      // مختصات شروع Y
    int width;        // عرض اتاق
    int height;       // ارتفاع اتاق
    int visited;  //بازیکن در این اتاق بوده یا نه 
    int floor; //طبقه ی اتاق
    char theme;
} Room;

void display_leaderboard();
int compare_users_by_points();
void sort_users_by_points();
int load_users_from_file();
void show_profile();
void new_game();
void continue_game();
void save_game_to_binary_file();
void load_game_from_binary_file();
void Scoreboard();
void show_message(WINDOW *msg_win, const char *message);

#endif