#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <locale.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "game.h"
#include "startup.h"
#include "menu.h"
#include "create_map.h"

void save_game_to_binary_file(char **map, int rows, int cols, Room *rooms, int num_rooms, Player *player,  int **map_visited, Food foods[7], int num_monster, Monster* monsters) {
    FILE *file = fopen("previous_game.bin", "wb");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // ذخیره تعداد سطرها و ستون‌ها
    fwrite(&rows, sizeof(int), 1, file);
    fwrite(&cols, sizeof(int), 1, file);

    // ذخیره نقشه
    for (int i = 0; i < rows; i++) {
        fwrite(map[i], sizeof(char), cols, file);
    }

    // ذخیره map_visited
    for (int i = 0; i < rows; i++) {
        fwrite(map_visited[i], sizeof(int), cols, file);
    }


    // ذخیره تعداد اتاق‌ها
    fwrite(&num_rooms, sizeof(int), 1, file);

    // ذخیره اطلاعات اتاق‌ها
    fwrite(rooms, sizeof(Room), num_rooms, file);

    // ذخیره اطلاعات بازیکن
    fwrite(player, sizeof(Player), 1, file);

    //ذخیره غذا ها
    fwrite(foods, sizeof(Food), 7, file);

    // ذخیره تعداد هیولا
    fwrite(&num_monster, sizeof(int), 1, file);

    // ذخیره اطلاعات هیولاها
    fwrite(monsters, sizeof(Monster), num_monster, file);


    fclose(file);
}

void load_game_from_binary_file(char ***map, int *rows, int *cols, Room **rooms, int *num_rooms, Player *player, int ***map_visited, Food (*foods)[7], int* num_monster, Monster **monsters) {
    FILE *file = fopen("previous_game.bin", "rb");
    if (!file) {
        perror("Could not open file");
        return;
    }

    // خواندن تعداد سطرها و ستون‌ها
    fread(rows, sizeof(int), 1, file);
    fread(cols, sizeof(int), 1, file);

    // تخصیص حافظه برای نقشه
    *map = (char **)malloc(*rows * sizeof(char *));
    for (int i = 0; i < *rows; i++) {
        (*map)[i] = (char *)malloc(*cols * sizeof(char));
        fread((*map)[i], sizeof(char), *cols, file);
    }

    *map_visited = (int **)malloc(*rows * sizeof(int *));
    for (int i = 0; i < *rows; i++) {
        (*map_visited)[i] = (int *)malloc(*cols * sizeof(int));
    }
    // خواندن `map_visited`
    for (int i = 0; i < *rows; i++) {
        fread((*map_visited)[i], sizeof(int), *cols, file);
    }

    

    // خواندن تعداد اتاق‌ها
    fread(num_rooms, sizeof(int), 1, file);

    // تخصیص حافظه برای اتاق‌ها
    *rooms = (Room *)malloc(*num_rooms * sizeof(Room));
    fread(*rooms, sizeof(Room), *num_rooms, file);

    // خواندن اطلاعات بازیکن
    fread(player, sizeof(Player), 1, file);

    //خواندن غذاهای موجود در صفحه
    fread(foods, sizeof(Food), 7, file);

     // خواندن تعداد هیولاها
    fread(num_monster, sizeof(int), 1, file);

    // تخصیص حافظه برای هیولاها
    *monsters = (Monster *)malloc(*num_monster * sizeof(Monster));
    fread(*monsters, sizeof(Monster), *num_monster, file);
    fclose(file);
}