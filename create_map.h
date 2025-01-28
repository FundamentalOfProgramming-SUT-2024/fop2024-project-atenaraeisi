#ifndef CREATE_MAP
#define CREATE_MAP

#include <ncurses.h>
#include "game.h"

typedef struct {
    int start_x;      // مختصات شروع X
    int start_y;      // مختصات شروع Y
    int width;        // عرض اتاق
    int height;       // ارتفاع اتاق
    int visited;  //بازیکن در این اتاق بوده یا نه 
    int floor; //طبقه ی اتاق
} Room;

bool is_valid_position(char **map, int x, int y);
void connect_rooms(char **map, int x1, int y1, int x2, int y2);
char **create_map(int width, int height, int level_difficulty, Room *rooms, int num_rooms);
void display_map(char ** map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited );

#endif