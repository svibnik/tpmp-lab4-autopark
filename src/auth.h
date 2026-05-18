#ifndef AUTH_H
#define AUTH_H

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MAX_ROLE 20

typedef struct {
    char username[MAX_USERNAME];
    char role[MAX_ROLE];
} User;

int authenticate(const char* login, const char* password, User* current_user);
void logout(User* current_user);
int is_logged_in(User* current_user);

#endif