#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "globals.h"
#include "save.h"
#include "initializers.h"

void save_accounts_encrypted()
{
    FILE *f = fopen("data/accounts.dat", "wb");
    if (!f) return;

    size_t estimated_size = licznik * 150 + 1;
    char *buffer = malloc(estimated_size);
    if (!buffer) { fclose(f); return; }
    buffer[0] = '\0';

    char line[100];
    for (int i = 0; i < licznik; i++) {
        sprintf(line, "%s %s %.2f\n", tab[i].num, tab[i].pin, tab[i].balance);
        strcat(buffer, line);
    }

    size_t data_len = strlen(buffer);
    xor_cipher(buffer, data_len);
    fwrite(buffer, 1, data_len, f);

    fclose(f);
    free(buffer);
}

void save_state()
{
    // --- BLOKOWANIE SYGNAŁÓW ---
    sigset_t mask, oldmask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    save_accounts_encrypted();

    FILE *cash_file = fopen("data/cash.txt", "w");
    if (cash_file) {
        for (int i = 0; i < atm_count; i++) {
            fprintf(cash_file, "%d - %d\n", atm_money[i].value, atm_money[i].amount);
        }
        fclose(cash_file);
    }

    // --- PRZYWRÓCENIE SYGNAŁÓW ---
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}