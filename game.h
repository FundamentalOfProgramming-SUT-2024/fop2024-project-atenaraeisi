#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

typedef struct{
    int x;
    int y;
    char color;
}Player;


void show_profile();
void new_game(char hero_color, int level_difficulty);
void continue_game();
void Scoreboard();
void show_message(WINDOW *msg_win, const char *message);

#endif