#include "globals.h"
#include "validations.h"
#include <string.h>
#include <ctype.h>

int validate_pin(const char *input, int account_index) {
    int len = strlen(input);
    if (len != 4) return -1; // PIN musi mieć 4 cyfry

    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)input[i])) return -2;
    }

    if (strcmp(tab[account_index].pin, input) == 0) return 0;
    return -3;
}

int validate_card_number(const char *input) {
    int len = strlen(input);
    if (len != 16) return -1; // Numer karty musi mieć 16 cyfr

    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)input[i])) return -2;
    }

    for (int i = 0; i < licznik; i++) {
        if (strcmp(tab[i].num, input) == 0) return i;
    }
    return -3;
}