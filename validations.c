#include "globals.h"
#include "validations.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int validate_pin(const char *input, int account_index)
{
    int len = strlen(input);

    // czy ma długość 4
    /* if (len != 4)
    {
        return -1;
    } */

    // czy to same cyfry
    for (int i = 0; i < len - 1; i++)
    {
        if (!isdigit((unsigned char)input[i]))
        {
            return -2;
        }
    }

    if (strcmp(tab[account_index].pin, input) == 0)
    {
        return 0;
    }

    return -3;
}

int validate_card_number(const char *input)
{
    int len = strlen(input);
    // sprawdzenie długości
    /* if (len != 16)
    {
        return -1;
    } */

    // czy to same cyfry
    for (int i = 0; i < len - 1; i++)
    {
        if (!isdigit((unsigned char)input[i]))
        {
            return -2;
        }
    }

    for (int i = 0; i < licznik; i++)
    {
        if (strcmp(tab[i].num, input) == 0)
        {
            return i;
        }
    }

    return -3;
}