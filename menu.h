#ifndef MENU_H
#define MENU_H

typedef struct user{
    char UserName[100];
    char password[100];
    char email[100];
    int rank;
    int points;
    int golds;
    int times_played;
}user;

extern user *user1; 
extern int have_account;
extern int level_difficulty;
extern char hero_color;

void save_user(user *new_user);
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
