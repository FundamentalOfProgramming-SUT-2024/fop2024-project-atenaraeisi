#ifndef AUTH_H
#define AUTH_H


void creat_account(int rows, int cols);
void log_in(int rows, int cols);
int check_to_create_username(const char str[]);
int check_to_create_password(const char str[]);
int check_email(const char email[]);
int check_username(const char str[], char* user1);
int check_password(const char str[], char user1[]);

#endif
