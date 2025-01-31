#ifndef CREATE_MAP
#define CREATE_MAP

#include <ncurses.h>
#include "game.h"

bool is_valid_position(char **map, int x, int y);
void connect_rooms(char **map, int x1, int y1, int x2, int y2);
char **create_map(int width, int height, int level_difficulty, Player* player, Room *rooms, int num_rooms, Food foods[7];);
void display_map(char ** map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited );
void display_whole_map(char ** map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited);

#endif