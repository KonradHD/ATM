#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

#define SECRET_KEY "PRIVATE_KEY"

void xor_cipher(char *data, size_t length)
{
    size_t key_len = strlen(SECRET_KEY);
    for (size_t i = 0; i < length; i++)
    {
        // XOR
        data[i] = data[i] ^ SECRET_KEY[i % key_len];
    }
}

void init_accounts()
{
    tab = malloc(sizeof(Account));
    int capacity = 1;

    FILE *f = fopen("data/accounts.txt", "r");
    if (!f)
    {
        perror("Nie mozna otworzyc pliku");
        free(tab);
        return;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, f) != -1)
    {
        if (licznik >= capacity)
        {
            capacity *= 2;
            tab = realloc(tab, capacity * sizeof(Account));
        }

        if (sscanf(line, "%29s %9s %lf", tab[licznik].num,
                   tab[licznik].pin, &tab[licznik].balance) == 3)
        {
            tab[licznik].isBlocked = 0;
            licznik++;
        }
    }

    free(line);
    fclose(f);
}

void init_atm_money()
{
    FILE *f = fopen("data/cash.txt", "r");

    if (!f)
    {
        printf("Brak pliku cash.txt - tworzenie bazy domyslnej...\n");
        return;
    }

    int atm_capacity = 4;
    atm_money = malloc(atm_capacity * sizeof(Cash));
    atm_count = 0;

    char buffer[100];

    while (fgets(buffer, sizeof(buffer), f))
    {
        if (atm_count >= atm_capacity)
        {
            atm_capacity *= 2;
            Cash *temp = realloc(atm_money, atm_capacity * sizeof(Cash));
            if (!temp)
            {
                perror("Blad alokacji pamieci!");
                fclose(f);
                return;
            }
            atm_money = temp;
        }

        int val, amt;
        if (sscanf(buffer, "%d - %d", &val, &amt) == 2)
        {
            atm_money[atm_count].value = val;
            atm_money[atm_count].amount = amt;
            atm_count++;
        }
    }

    fclose(f);
    printf("Wczytano %d rodzajow banknotow do bankomatu.\n", atm_count);
}

void init_accounts_encrypted()
{
    FILE *f = fopen("data/accounts.dat", "rb");
    if (!f)
        return;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);

    if (file_size > 0)
    {
        char *buffer = malloc(file_size + 1);
        fread(buffer, 1, file_size, f);
        buffer[file_size] = '\0';
        fclose(f);

        xor_cipher(buffer, file_size);

        char *cursor = buffer;
        int offset;

        int capacity = 1;
        tab = malloc(sizeof(Account));

        while (sscanf(cursor, "%29s %9s %lf%n",
                      tab[licznik].num,
                      tab[licznik].pin,
                      &tab[licznik].balance,
                      &offset) == 3)
        {
            tab[licznik].isBlocked = 0;
            cursor += offset;
            licznik++;

            if (licznik >= capacity)
            {
                capacity *= 2;
                tab = realloc(tab, capacity * sizeof(Account));
            }
        }
        free(buffer);
    }
    else
    {
        fclose(f);
    }
}