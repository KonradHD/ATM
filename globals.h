#ifndef GLOBALS_H
#define GLOBALS_H

typedef struct
{
    char num[30];
    char pin[10];
    double balance;
    int isBlocked;
} Account;

typedef struct
{
    int value;
    int amount;
} Cash;

extern int atm_count;
extern int licznik;

extern Cash *atm_money;
extern Account *tab;

#define KEY_SIZE 32
#define IV_SIZE 16
#define DEFAULT_KEY_PATH "config/secret.key"
#define ACCOUNTS_FILE_PATH "data/accounts.enc"

#endif
