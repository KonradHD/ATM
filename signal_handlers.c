#include <stdio.h>
#include <stdlib.h>

#include "globals.h"
#include "file_manager.h"
#include "signal_handlers.h"

void handle_sigint(int sig)
{
    printf("\n\nOtrzymano sygnal przerwania! Zapisuje dane...\n");

    save_state();

    if (tab)
        free(tab);
    if (atm_money)
        free(atm_money);

    printf("Zamykanie programu.\n");
    exit(0);
}

void handle_timeout(int sig)
{
    printf("\n\n!!! CZAS MINAL !!!\n");
    printf("Brak aktywnosci przez 30 sekund. Wylogowywanie...\n");

    save_state();

    if (tab)
        free(tab);
    if (atm_money)
        free(atm_money);
    exit(0);
}