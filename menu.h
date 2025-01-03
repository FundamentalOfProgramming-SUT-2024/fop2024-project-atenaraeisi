#ifndef MENU_H
#define MENU_H

typedef struct user{
    char UserName[50];
    char password[50];
    char email[50];
}user;

extern user *user1; 

void game_name(int row, int col);
void print_border(int rows, int cols);
void game_menu(int rows, int cols, int selected);
void Menu(int rows, int cols, int *selected, int num_items);
void GameMenu(int rows, int cols, int *selected, int num_items);
void print_main_menu(int row, int col, int selected);
void print_game_menu(int row, int col, int selected);
void print_login_menu(int row, int col, int selected);
void new_game();
void continue_game();
void Scoreboard();
void Settings();
void show_profile();

#endif
