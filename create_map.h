#ifndef CREATE_MAP
#define CREATE_MAP

#include <ncurses.h>

typedef struct {
    int start_x;      // مختصات شروع X
    int start_y;      // مختصات شروع Y
    int width;        // عرض اتاق
    int height;       // ارتفاع اتاق
    int visited;
} Room;

bool is_valid_position(char **map, int x, int y);
void connect_rooms(char **map, int x1, int y1, int x2, int y2);
char **create_map(int width, int height, int level_difficulty);
void display_map(char ** map, int width, int height, char hero_color);

#endif