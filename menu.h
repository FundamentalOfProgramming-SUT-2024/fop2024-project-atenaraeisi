#ifndef MENU_H
#define MENU_H

typedef struct user{
    char UserName[50];
    char password[50];
    char email[50];
}user;

extern user *user1; 
extern int level_difficulty;
extern char hero_color;

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
void show_profile();
void Settings();
void print_settings_menu();
void defficulty_level(int rows, int cols, int *selected, int num_items);
void print_difficulty_level(int row, int col, int selected);
void print_hero_color(int row, int col, int selected);
void change_hero_color(int rows, int cols, int *selected, int num_items);
void change_background_music(int rows, int cols, int *selected, int num_items);

#endif
