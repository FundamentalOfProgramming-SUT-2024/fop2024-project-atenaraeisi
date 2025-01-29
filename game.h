#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

typedef struct{
    int x;
    int y;
    char color;
    char direction[2];
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
void save_game_to_binary_file(char **map, int rows, int cols, Room *rooms, int num_rooms, Player *player,  int **map_visited);
void load_game_from_binary_file(char ***map, int *rows, int *cols, Room **rooms, int *num_rooms, Player *player, int ***map_visited);
void Scoreboard();
void show_message(WINDOW *msg_win, const char *message);

#endif