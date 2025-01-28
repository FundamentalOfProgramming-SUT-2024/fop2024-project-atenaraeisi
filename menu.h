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

void game_menu();
void Menu();
void GameMenu();
void print_main_menu();
void print_game_menu();
void print_login_menu();
void Settings();
void print_settings_menu();
void defficulty_level();
void print_difficulty_level();
void print_hero_color();
void change_hero_color();
void change_background_music();

#endif
