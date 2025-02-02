#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

typedef struct {
    int x;          // مختصات x غذا
    int y;          // مختصات y غذا
    int type;       // نوع غذا (۱: معمولی، ۲: اعلا، ۳: جادویی، ۴: فاسد)
    char name[20]; // نام غذا (مثلاً "نان")
    int restore_hunger;
    int restore_health;
} Food;

typedef struct {
    char symbol;
    char *name;
    int health;
    int damage;
    int x, y;          // موقعیت فعلی هیولا
    int type;          // نوع هیولا برای تعیین رفتار
    int stepsTaken;    // تعداد قدم‌های برداشته‌شده
    bool active;       // آیا هیولا هنوز قهرمان را دنبال می‌کند؟
} Monster;


// تعریف انواع سلاح
typedef enum {
    MACE,
    DAGGER,
    MAGIC_WAND,
    NORMAL_ARROW,
    SWORD
} WeaponType;

// ساختار برای سلاح
typedef struct {
    WeaponType type;
    char symbol_on_map;
    char name[20];
    char symbol[5];  // برای نگه‌داری کاراکترهای Unicode
    int damage;
    int range;
    int is_melee;
    int numbers;
    int is_consumable;
} Weapon;

typedef struct {
    int dx, dy; // جهت حرکت
    Weapon weapon_used; // سلاح استفاده شده
} LastShot;

typedef enum {
    HEALTH,
    SPEED,
    DAMAGE
} TalismanType;

//طلسم
typedef struct {
    TalismanType type;
    char name[20];
    char symbol[5];  // برای نگه‌داری کاراکترهای Unicode
    int lifetime;
    int active;
} talisman;

typedef struct{
    int x;
    int y;
    char color;
    char direction[2];
    int is_in_floor;
    int collected_golds;
    int points;
    int lives;
    int health;
    int health_increaser;
    int health_speed;
    int speed;
    int power;
    int hunger; // سطح گرسنگی
    Food inventory[5];
    int food_count;
    Weapon Weapon_list[10];
    int Weapon_count;
    Weapon equipped_weapon;
    talisman talisman_list[10];
    talisman current_talisman;
    int talisman_count;
}Player;

typedef struct {
    int start_x;      // مختصات شروع X
    int start_y;      // مختصات شروع Y
    int width;        // عرض اتاق
    int height;       // ارتفاع اتاق
    int visited;  //بازیکن در این اتاق بوده یا نه 
    int floor; //طبقه ی اتاق
    char theme;
} Room;



void display_leaderboard();
int compare_users_by_points();
void sort_users_by_points();
int load_users_from_file();
void show_profile();
void new_game();
void continue_game();
void save_game_to_binary_file();
void load_game_from_binary_file();
int can_go(int y, int x, char **map, Player* player, int ***map_visited, int g_clicked, Food foods[7]);
void move_fast(char **map, int width, int height, Player *player, int **map_visited, Room *rooms, int num_rooms, int display_completely, Food foods[7]);
void Scoreboard();
void initialize_player(Player *player);
int calculate_num_rooms(int level_difficulty);
int **initialize_map_visited(int width, int height);
void free_map_visited(int **map_visited, int height);
void initialize_player_position(Player *player, Room *rooms, char **map);
void draw_hud(int width, int whole_height, Player player);
void show_message(WINDOW *msg_win, const char *message);
void display_game_state(char **map, int width, int height, Player player, Room *rooms, int num_rooms, int **map_visited, int display_completely, int whole_height);
void handle_movement(int ch, Player *player, char **map, int **map_visited, int g_clicked, Food *foods, int width, int height);
void draw_ui(int whole_height, int width, Player player);
void free_allocated_memory(char **map, int **map_visited, Room *rooms, int height);
#endif