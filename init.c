#include <stdio.h>

typedef struct {
    char nr[30];
    char date[10];
    int cvv;
    double balance;
} Account;

int main() {
    FILE *f = fopen("accounts.txt", "wb");
    Account items[] =
    {
        {"74812950634178290516345281", "06/27", 123,2034.43},
        {"90341657829013475620984573", "07/28", 345,22444}
    };

    fwrite(items, sizeof(Account), 2, f);
    fclose(f);

    return 0;
}