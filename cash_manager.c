#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include "globals.h"
#include "cash_manager.h"
#include "history.h"

void handle_deposit(int index) {
    double wplata;
    printf("Podaj kwotę do wpłaty (wielokrotność 10): ");
    alarm(30);
    if (scanf("%lf", &wplata) != 1) {
        while (getchar() != '\n');
        alarm(0);
        return;
    }
    alarm(0);

    if (wplata > 0 && wplata <= 2000 && (fabs(fmod(wplata, 10.0)) < 1e-9)) {
        update_atm_cash((int)wplata, 0);
        tab[index].balance += wplata;
        printf("Sukces! Nowe saldo: %.2f\n", tab[index].balance);

        char msg[100];
        sprintf(msg, "Wplata: +%.2f. Saldo: %.2f", wplata, tab[index].balance);
        log_activity(tab[index].num, msg);
    } else {
        printf("Błąd kwoty!\n");
    }
}

void handle_withdrawal(int index) {
    double wyplata;
    printf("Podaj kwotę do wypłaty: ");
    alarm(30);
    if (scanf("%lf", &wyplata) != 1) {
        while (getchar() != '\n');
        alarm(0);
        return;
    }
    alarm(0);

    if (wyplata > 0 && wyplata <= 2000 && wyplata <= tab[index].balance &&
        (fabs(fmod(wyplata, 10.0)) < 1e-9) && check_can_withdraw((int)wyplata)) {

        update_atm_cash((int)wyplata, 1);
        tab[index].balance -= wyplata;
        printf("Wypłacono %.2f. Nowe saldo: %.2f\n", wyplata, tab[index].balance);

        char msg[100];
        sprintf(msg, "Wyplata: -%.2f. Saldo: %.2f", wyplata, tab[index].balance);
        log_activity(tab[index].num, msg);
        } else {
            printf("Błąd: Nie można zrealizować wypłaty.\n");
        }
}