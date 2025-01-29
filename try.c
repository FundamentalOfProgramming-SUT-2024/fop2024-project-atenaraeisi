#include <stdio.h>
#include <stdlib.h>

typedef struct user {
    char UserName[100];
    char password[100];
    char email[100];
    int rank;
    int points;
    int golds;
    int times_played;
} user;

// تابع ذخیره‌سازی
void save_user(user *new_user) {
    FILE *file = fopen("users.bin", "ab"); // "ab" برای افزودن به فایل باینری
    if (!file) {
        perror("Error opening file");
        return;
    }

    // ذخیره محتوای ساختاری که پوینتر به آن اشاره می‌کند
    fwrite(new_user, sizeof(user), 1, file); 
    fclose(file);
}

// تابع خواندن
int load_users_from_file(const char *filename, user **users) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    int total_users = file_size / sizeof(user);
    *users = (user *)malloc(total_users * sizeof(user));
    if (!*users) {
        perror("Memory allocation error");
        fclose(file);
        return 0;
    }

    fread(*users, sizeof(user), total_users, file);
    fclose(file);

    return total_users;
}

// تابع تست
int main() {
    const char *filename = "users.bin";

    // ذخیره یک کاربر جدید
    user new_user = {"JohnDoe", "password123", "johndoe@example.com", 1, 100, 5, 10};
    save_user(&new_user);

    // خواندن کاربران ذخیره‌شده
    user *users = NULL;
    int total_users = load_users_from_file(filename, &users);

    // نمایش کاربران
    printf("Total users: %d\n", total_users);
    for (int i = 0; i < total_users; i++) {
        printf("User %d: %s, Points: %d, Golds: %d, Times Played: %d\n",
               i + 1, users[i].UserName, users[i].points, users[i].golds, users[i].times_played);
    }

    free(users);
    return 0;
}
