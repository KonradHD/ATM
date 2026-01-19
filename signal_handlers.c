#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "save.h"
#include "signal_handlers.h"
#include "history.h"

void handle_sigint(int sig)
{
    printf("\n\nOtrzymano sygnal przerwania! Zapisuje dane...\n");

    if (current_user_index != -1 && tab != NULL) {
        log_activity(tab[current_user_index].num, "Wylogowano przez przerwanie programu (Ctrl+C).");
    }

    save_state();

    if (tab) free(tab);
    if (atm_money) free(atm_money);

    printf("Zamykanie programu.\n");
    exit(0);
}

void handle_timeout(int sig)
{
    printf("\n\n!!! CZAS MINAL !!!\n");

    if (current_user_index != -1 && tab != NULL) {
        log_activity(tab[current_user_index].num, "Wylogowano automatycznie (Timeout).");
    }

    save_state();

    if (tab) free(tab);
    if (atm_money) free(atm_money);
    exit(0);
}