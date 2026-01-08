#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "save.h"
#include "initializers.h"

void save_accounts()
{

    FILE *account_file = fopen("data/accounts.txt", "w");
    if (account_file)
    {
        for (int i = 0; i < licznik; i++)
        {
            fprintf(account_file, "%s %s %.2f\n",
                    tab[i].num, tab[i].pin, tab[i].balance);
        }
        fclose(account_file);
    }
}

void save_accounts_encrypted()
{

    FILE *f = fopen("data/accounts.dat", "wb");
    if (!f)
        return;

    size_t estimated_size = licznik * 100 + 1;
    char *buffer = malloc(estimated_size);
    buffer[0] = '\0';

    char line[100];

    for (int i = 0; i < licznik; i++)
    {
        sprintf(line, "%s %s %.2f\n", tab[i].num, tab[i].pin, tab[i].balance);
        strcat(buffer, line);
    }

    size_t data_len = strlen(buffer);
    xor_cipher(buffer, data_len);

    fwrite(buffer, 1, data_len, f);

    fclose(f);
    free(buffer);
    printf("Baza danych zostala zaszyfrowana i zapisana.\n");
}

void save_state()
{
    save_accounts_encrypted();

    FILE *cash_file = fopen("data/cash.txt", "w");

    if (cash_file)
    {
        for (int i = 0; i < atm_count; i++)
        {
            fprintf(cash_file, "%d - %d\n", atm_money[i].value, atm_money[i].amount);
        }
        fclose(cash_file);
    }
    else
    {
        perror("Blad zapisu pliku cash.txt");
    }
}
