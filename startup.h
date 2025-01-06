#ifndef STARTUP_H
#define STARTUP_H

#include <ncurses.h>

void startup();
extern int rows, cols;
void dimensions();
void game_name();
void print_border();

#endif