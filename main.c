#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>

typedef struct user{
    char UserName[50];
    char password[50];
    char email[50];
}user;

user *user1 = NULL;

void game_name(int row, int col){
    attron(A_BOLD);
    char message[] = "Welcome to ROGUE!";
    mvprintw((row / 2)-1, (col - strlen(message)) / 2, "%s", message); // نمایش پیام در مرکز
    attroff(A_BOLD);

    char credit[] = "Made by Atena :)";
    mvprintw((row/2)+1 , (col - strlen(credit))/2 ,"%s", credit);
    refresh();                             
    sleep(2);                         
    clear(); 
}

void print_border(int rows, int cols){
    for (int i = 0; i < cols; i++) {
        mvaddch(0, i, '=');            // خط بالا
        mvaddch(rows - 1, i, '=');     // خط پایین
    }
    for (int i = 0; i < rows; i++) {
        mvaddstr(i, 0, "||");            // ستون سمت چپ
        mvaddstr(i, cols - 2, "||");     // ستون سمت راست
    }
    refresh();
}
void move_in_menu(int rows, int cols, int *selected, int num_items);

void print_main_menu(int row, int col, int selected){
    attron(A_BOLD);
    mvprintw(((row-11 )/ 2), (col - strlen("Menuuuuuuuuu")) / 2, "Menu");
    attroff(A_BOLD);
    
    char *menu_items[] = {
        "1. Make a new account",
        "2. Log in ",
        "3. Start playing",
        "4. Profile",
        "5. Score"
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


int check_to_create_username(const char str[]){
    FILE* users_info = fopen("users_info.txt", "r");

    if (users_info == NULL) {
        return 1;  // در صورت بروز خطا در باز کردن فایل
    }

    char line[1024];  // یک بافر برای خواندن هر خط از فایل
    while (fgets(line, sizeof(line), users_info)) {
        // بررسی اینکه آیا رشته مورد نظر در خط فعلی پیدا شده است یا نه
        if (strstr(line, str)) {
            fclose(users_info);
            return 1;  // اگر رشته پیدا شد، 1 برگردانده می‌شود
        }
    }

    fclose(users_info);
    return 0;  // اگر رشته پیدا نشد، 0 برگردانده می‌شود
}
int check_to_create_password(const char str[]){
    if (strlen(str) < 7) return 0; // طول کمتر از 7 کاراکتر
    int has_upper = 0, has_lower = 0, has_digit = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (isupper(str[i])) has_upper = 1;
        else if (islower(str[i])) has_lower = 1;
        else if (isdigit(str[i])) has_digit = 1;
    }
    return has_upper && has_lower && has_digit;
}
int check_email(const char email[]) {
    // بررسی وجود @
    const char *at_ptr = strchr(email, '@');
    if (at_ptr == NULL) {
        return 0; // @ وجود ندارد
    }

    // بررسی وجود نقطه بعد از @
    const char *dot_ptr = strchr(at_ptr, '.');
    if (dot_ptr == NULL) {
        return 0; // نقطه بعد از @ وجود ندارد
    }

    // اطمینان از اینکه چیزی قبل از @ وجود دارد
    if (at_ptr == email) {
        return 0; // نام کاربری وجود ندارد
    }

    // اطمینان از اینکه چیزی بین @ و . وجود دارد
    if (dot_ptr - at_ptr <= 1) {
        return 0; // دامنه وجود ندارد
    }

    // اطمینان از اینکه چیزی بعد از . وجود دارد
    if (strlen(dot_ptr + 1) < 1) {
        return 0; // پسوند وجود ندارد
    }

    // اطمینان از معتبر بودن کاراکترها (حروف انگلیسی و عدد)
    for (const char *ptr = email; ptr < at_ptr; ptr++) {
        if (!isalnum(*ptr)) {
            return 0; // نام کاربری شامل کاراکترهای غیرمجاز است
        }
    }

    for (const char *ptr = at_ptr + 1; ptr < dot_ptr; ptr++) {
        if (!isalpha(*ptr)) {
            return 0; // دامنه شامل کاراکترهای غیرمجاز است
        }
    }

    for (const char *ptr = dot_ptr + 1; *ptr != '\0'; ptr++) {
        if (!isalpha(*ptr)) {
            return 0; // پسوند شامل کاراکترهای غیرمجاز است
        }
    }

    return 1; // ایمیل معتبر است
}

void creat_account(int rows, int cols,  int *selected, int num_items){
    echo();
    clear();
    print_border(rows, cols);
    refresh();

    user1 = (user *) malloc(sizeof(user));

    move(4,4);
    printw("Enter your username: ");
    refresh();
    getstr(user1->UserName);
    while(check_to_create_username(user1->UserName)){
        attron(COLOR_PAIR(2));
        mvprintw(5, 4, "This username is already existed!");
        attroff(COLOR_PAIR(2));
        refresh();
        sleep(1.5);
        clear();
        print_border(rows, cols);
        refresh();
        move(4,4);
        printw("Enter your username: ");
        refresh();
        getstr(user1->UserName);
    }

    move(6,4);
    printw("Enter your password: ");
    refresh();
    getstr(user1->password);
    while(strlen(user1->password)<7 || !check_to_create_password(user1->password)){
        attron(COLOR_PAIR(2));
        mvprintw(7, 4, "Password must have at least 7 characters,");
        mvprintw(8, 4, "1 capital letter, 1 small letter and 1 number!");
        attroff(COLOR_PAIR(2));
        refresh();
        sleep(1.5);
        clear();
        print_border(rows, cols);
        refresh();
        move(4,4);
        printw("Enter your username: %s", user1->UserName);
        move(6,4);
        printw("Enter your password: ");
        refresh();
        getstr(user1->password);
    }

    move(8, 4);
    printw("Enter your email: ");
    refresh();
    getstr(user1->email);
    while(!check_email(user1->email)){
        attron(COLOR_PAIR(2));
        mvprintw(9, 4, "Enter your email in a correct way!");
        attroff(COLOR_PAIR(2));
        refresh();
        sleep(1.5);
        clear();
        print_border(rows, cols);
        refresh();
        move(4,4);
        printw("Enter your username: %s", user1->UserName);
        move(6,4);
        printw("Enter your password: %s", user1->password);
        move(8, 4);
        printw("Enter your email: ");
        refresh();
        getstr(user1->email);
    }

    FILE* users_info = fopen("users_info.txt", "a");

    fprintf(users_info, "%s: %s: %s\n", user1->UserName, user1->password, user1->email);
    fclose(users_info);
    noecho();

    free(user1);

    move(10, 4);
    printw("Account created successfully!");
    refresh();
    sleep(2);
    move_in_menu(rows, cols, selected, num_items);

}
1
int check_username(const char str[], char* user1){
    FILE* users_info = fopen("users_info.txt", "r");

    if (users_info == NULL) {
        return 1;
    }

    char line[1024];  // یک بافر برای خواندن هر خط از فایل
    while (fgets(line, sizeof(line), users_info)) {
        // بررسی اینکه آیا رشته مورد نظر در خط فعلی پیدا شده است یا نه
        if (strstr(line, str) != NULL ){
            strncpy(user1, line, 1024);
            fclose(users_info);
            return 0;  // اگر رشته پیدا شد، 1 برگردانده می‌شود
        }
    }

    fclose(users_info);
    return 1;
}
int check_password(const char str[], char user1[]){

    char first[50], middle[50], last[50];

    sscanf(user1, "%[^:]: %[^:]: %s", first, middle, last);
    
    if (strcmp(middle, str) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void log_in(int rows, int cols,  int *selected, int num_items){
    echo();
    clear();
    print_border(rows, cols);
    refresh();

    user1 = (user *) malloc(sizeof(user));
    char info_of_1_user[1024];

    move(4,4);
    printw("Enter your username: ");
    refresh();
    getstr(user1->UserName);
    while(check_username(user1->UserName, info_of_1_user)){
        attron(COLOR_PAIR(2));
        mvprintw(5, 4, "This username doesn't exist!");
        attroff(COLOR_PAIR(2));
        refresh();
        sleep(1.5);
        clear();
        print_border(rows, cols);
        refresh();
        move(4,4);
        printw("Enter your username: ");
        refresh();
        getstr(user1->UserName);
    }

    move(6,4);
    printw("Enter your password: ");
    refresh();
    getstr(user1->password);
    while(check_password(user1->password, info_of_1_user)){
        attron(COLOR_PAIR(2));
        mvprintw(7, 4, "Password isn't correct!");
        attroff(COLOR_PAIR(2));
        refresh();
        sleep(1.5);
        clear();
        print_border(rows, cols);
        refresh();
        move(4,4);
        printw("Enter your username: %s", user1->UserName);
        move(6,4);
        printw("Enter your password: ");
        refresh();
        getstr(user1->password);
    }

    move(10, 4);
    printw("Welcome %s!", user1->UserName);
    refresh();
    sleep(2);
    free(user1);
    move_in_menu(rows, cols, selected, num_items);

}

void print_login_menu(int row, int col, int selected){
    char *menu_items[] = {
        "1. Continue as Guest",
        "2. Log in to an existing account"
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

void start_game(){

    
}

void show_profile(){

    
}

void show_score(){

    
}

void move_in_menu(int rows, int cols, int *selected, int num_items){
    while (1) {
        clear();
        print_border(rows, cols);
        print_main_menu(rows, cols, *selected);
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

    switch (*selected){
        case 0:
            creat_account(rows, cols, selected, num_items);
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
                    log_in(rows, cols, selected, num_items);
                    start_game();
                    break;

                case 0:
                    start_game();
                    break;
            }
            break;
        
        case 2:
            start_game();
            break;

        case 3:
            show_profile();
            break;

        case 4:
            show_score();
            break;
    }

}

int main() {
    initscr();             
    noecho();              
    curs_set(FALSE);
    keypad(stdscr, TRUE); 

    

    if (has_colors()){
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_CYAN);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_BLUE, COLOR_YELLOW);
    }      

    // گرفتن ابعاد ترمینال
    int rows, cols;
    getmaxyx(stdscr, rows, cols);  

    // نوشتن اسم بازی در شروع
    game_name(rows, cols);

    //menu.............................................
    int selected = 0; // گزینه انتخاب شده در منو
    int num_items = 5;
    move_in_menu(rows, cols, &selected, num_items);
    //..................................................

    
                       
    endwin();             
    return 0;
}
