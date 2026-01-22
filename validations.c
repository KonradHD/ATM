#include "globals.h"
#include "validations.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>

int validate_pin(const char *input, int account_index)
{
    char clean_input[32];
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        if (!isspace((unsigned char)input[i]))
        {
            if (j < 31)
            {
                clean_input[j++] = input[i];
            }
        }
    }
    clean_input[j] = '\0';

    int len = strlen(clean_input);
    if (len != 4)
    {
        return -1; // PIN musi mieć 4 cyfry
    }
    for (int i = 0; i < len; i++)
    {
        if (!isdigit((unsigned char)input[i]))
        {
            return -2;
        }
    }

    if (strcmp(tab[account_index].pin, input) == 0)
        return 0;
    return -3;
}

int validate_card_number(const char *input)
{
    int len = strlen(input);
    if (len != 16)
        return -1; // Numer karty musi mieć 16 cyfr

    for (int i = 0; i < len; i++)
    {
        if (!isdigit((unsigned char)input[i]))
            return -2;
    }

    for (int i = 0; i < licznik; i++)
    {
        if (strcmp(tab[i].num, input) == 0)
            return i;
    }
    return -3;
}

int check_luhn(const char *raw_input, int is_user_validation)
{
    // 1. KROK: Czyszczenie danych (pozbywamy się spacji i \n przed obliczeniami)
    char account_number[32];
    int j = 0;

    for (int i = 0; raw_input[i] != '\0'; i++)
    {
        // Pomijamy białe znaki (spacja, \n, \r, \t) oraz myślniki
        if (!isspace((unsigned char)raw_input[i]) && raw_input[i] != '-')
        {
            if (j < 31)
            {
                account_number[j++] = raw_input[i];
            }
        }
    }
    account_number[j] = '\0'; // Zamykamy czysty string

    int len = strlen(account_number);
    int sum = 0;
    int is_second = 0; // Flaga oznaczająca co drugą cyfrę (zaczynając od prawej)

    // Pętla od ostatniej cyfry do pierwszej (od prawej do lewej)
    for (int i = len - 1; i >= 0; i--)
    {
        char c = account_number[i];

        // Opcjonalnie: Ignoruj spacje lub myślniki
        if (c == ' ' || c == '-')
        {
            continue;
        }

        // Sprawdź, czy znak jest cyfrą
        if (!isdigit(c))
        {
            return -2; // Nieprawidłowy znak w numerze
        }

        // Konwersja char na int (ASCII '0' to 48)
        int digit = c - '0';

        if (is_second)
        {
            digit *= 2;
            // Jeśli podwojona wartość jest większa niż 9, sumujemy jej cyfry
            // (to matematycznie to samo co odjęcie 9, np. 16 -> 1+6=7 oraz 16-9=7)
            if (digit > 9)
            {
                digit -= 9;
            }
        }

        sum += digit;

        // Zmień flagę dla następnej iteracji
        is_second = !is_second;
    }

    sum += 10 - sum % 10;

    if (sum % 10 != 0)
    {
        return -1;
    }

    if (is_user_validation == 0)
    {
        return 0;
    }

    for (int i = 0; i < licznik; i++)
    {
        if (strcmp(tab[i].num, account_number) == 0)
            return i;
    }

    return -3;
}
