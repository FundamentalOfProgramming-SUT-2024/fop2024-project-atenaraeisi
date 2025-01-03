#include "auth.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>  
#include <unistd.h> 
#include <stdlib.h>
#include <ctype.h>
#include "menu.h"

user *user1 = NULL;

void creat_account(int rows, int cols){
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

}

void log_in(int rows, int cols){
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