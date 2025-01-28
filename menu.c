#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>
#include "menu.h"
#include "auth.h"
#include "game.h"
#include "startup.h"

int level_difficulty = 2;
char hero_color = 'r';



void print_main_menu(int row, int col, int selected){
    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("Menuuuuuuuuu")) / 2, "Menu");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "1. Make a new account",
        "2. Log in ",
        "3. Start playing"
    };
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen("2. Log in to an existing account")) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                             
}

void print_login_menu(int row, int col, int selected){
    char *menu_items[] = {
        "Continue as Guest",
        "Log in to an existing account"
    };
    int num_items = 2;

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen("2. Log in to an existing account")) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                             
}

void print_game_menu(int row, int col, int selected){
    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("Menuuuuuuuuu")) / 2, "GAME MENU");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "Create a new game",
        "Continue previous game",
        "Scoreboard",
        "Settings",
        "Profile"
    };
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen(menu_items[i])) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                    
    
}
void GameMenu(int rows, int cols){
    int selected = 0;
    int num_items = 5;

    while (1) {
        clear();
        print_border();
        print_game_menu(rows, cols, selected);
        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            selected = (selected - 1 + num_items) % num_items; // بالا رفتن در منو
        } else if (ch == KEY_DOWN) {
            selected = (selected + 1) % num_items; // پایین رفتن در منو
        } else if (ch == 10 || ch == KEY_ENTER) {
            break;
        }
    }

    switch (selected){
        case 0:
            new_game(hero_color, level_difficulty);
            break;

        case 1:
            continue_game();
            break;
        
        case 2:
            Scoreboard();
            break;

        case 3:
            int selected2 = 0;
            int num_items2 = 4;
            Settings(rows, cols, &selected2, num_items2);
            break;

        case 4:
            show_profile();
            break;
    }

}

void Menu(int rows, int cols){
    int selected =0;
    int num_items = 3;

    while (1) {
        clear();
        print_border(rows, cols);
        print_main_menu(rows, cols, selected);
        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            selected = (selected - 1 + num_items) % num_items; // بالا رفتن در منو
        } else if (ch == KEY_DOWN) {
            selected = (selected + 1) % num_items; // پایین رفتن در منو
        } else if (ch == 10 || ch == KEY_ENTER) {
            break;
        }
    }

    switch (selected){
        case 0:
            creat_account(rows, cols);
            GameMenu(rows, cols);
            break;

        case 1:
            int selected2=0;
            while (1) {
                clear();
                print_border(rows, cols);
                print_login_menu(rows, cols, selected2);
                refresh();

                int ch = getch();
                if (ch == KEY_UP) {
                    selected2 = (selected2 - 1 + 2) % 2; // بالا رفتن در منو
                } else if (ch == KEY_DOWN) {
                    selected2 = (selected2 + 1) % 2; // پایین رفتن در منو
                } else if (ch == 10 || ch == KEY_ENTER) {
                    break;
                }
            }
            switch (selected2){
                case 1:
                    log_in(rows, cols);
                    break;

                case 0:
                    break;
            }
            GameMenu(rows, cols);
            break;

        case 2:
            GameMenu(rows, cols);
            break;
    }

}
void print_settings_menu(int row, int col, int selected){
    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("Menuuuuuuuuu")) / 2, "SETTINGS");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "Difficulty Level",
        "Change Hero Color",
        "Select Background Music",
        "Game Menu"
    };
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen(menu_items[i])) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                    
    
}
void Settings(int rows, int cols, int *selected, int num_items){
    while (1) {
        clear();
        print_border(rows, cols);
        print_settings_menu(rows, cols, *selected);
        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            *selected = (*selected - 1 + num_items) % num_items; // بالا رفتن در منو
        } else if (ch == KEY_DOWN) {
            *selected = (*selected + 1) % num_items; // پایین رفتن در منو
        } else if (ch == 10 || ch == KEY_ENTER) {
            break;
        }
    }
    int selected2 = 0;
    int num_items2 = 4;
    int selected4=0;
    int num_items4=3;
    switch (*selected){
        
        case 0:
            
            defficulty_level(rows, cols, &selected4, num_items4);
            Settings(rows, cols, &selected2, num_items2);
            break;

        case 1:
            change_hero_color(rows, cols, &selected4, num_items4);
            Settings(rows, cols, &selected2, num_items2);
            break;
        
        case 2:
            change_background_music(rows, cols, &selected4, num_items4);
            Settings(rows, cols, &selected2, num_items2);
            break;
        
        case 3:
            GameMenu(rows, cols);
            break;

    }

}

void print_difficulty_level(int row, int col, int selected){

    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("CHOOSE DIFFICULTY LEVEL")) / 2, "CHOOSE DIFFICULTY LEVEL");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "Easy",
        "Medium",
        "Hard"
    };
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen(menu_items[i])) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                    
    
}
void defficulty_level(int rows, int cols, int *selected, int num_items){
    while (1) {
        clear();
        print_border(rows, cols);
        print_difficulty_level(rows, cols, *selected);
        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            *selected = (*selected - 1 + num_items) % num_items; // بالا رفتن در منو
        } else if (ch == KEY_DOWN) {
            *selected = (*selected + 1) % num_items; // پایین رفتن در منو
        } else if (ch == 10 || ch == KEY_ENTER) {
            break;
        }
    }
    int selected2 = 0;
    int num_items2 = 4;
    switch (*selected){
        case 0:
            level_difficulty=1;
            Settings(rows, cols, &selected2, num_items2);
            break;

        case 1:
            level_difficulty=2;
            Settings(rows, cols, &selected2, num_items2);
            break;
        
        case 2:
            level_difficulty=3;
            Settings(rows, cols, &selected2, num_items2);
            break;

    }

}

void print_hero_color(int row, int col, int selected){

    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("CHOOSE COLOR OF HERO")) / 2, "CHOOSE COLOR OF HERO");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "Red",
        "Green",
        "Blue"
    };
    int num_items = sizeof(menu_items) / sizeof(menu_items[0]);

    for (int i = 0; i < num_items; i++) {
        if (i == selected) {
            attron(A_REVERSE); // گزینه انتخاب شده
        }
        mvprintw(row / 2 - 3 + i, (col - strlen(menu_items[i])) / 2, "%s", menu_items[i]);
        if (i == selected) {
            attroff(A_REVERSE); // بازگرداندن حالت عادی
        }
    }

    refresh();                    
    
}
void change_hero_color(int rows, int cols, int *selected, int num_items){
    while (1) {
        clear();
        print_border(rows, cols);
        print_hero_color(rows, cols, *selected);
        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            *selected = (*selected - 1 + num_items) % num_items; // بالا رفتن در منو
        } else if (ch == KEY_DOWN) {
            *selected = (*selected + 1) % num_items; // پایین رفتن در منو
        } else if (ch == 10 || ch == KEY_ENTER) {
            break;
        }
    }
    int selected2 = 0;
    int num_items2 = 4;
    switch (*selected){
        case 0:
            hero_color = 'r';
            Settings(rows, cols, &selected2, num_items2);
            break;

        case 1:
            hero_color = 'g';
            Settings(rows, cols, &selected2, num_items2);
            break;
        
        case 2:
            hero_color = 'b';
            Settings(rows, cols, &selected2, num_items2);
            break;

    }

}

void change_background_music(int rows, int cols, int *selected, int num_items){

}