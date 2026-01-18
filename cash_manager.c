#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "cash_manager.h"
#include "history.h"

void update_atm_cash(int kwota, int isWithdrawal)
{
    int do_wydania = kwota;
    for (int i = atm_count - 1; i >= 0; i--)
    {
        int nominal = atm_money[i].value;
        int dostepne_sztuki = atm_money[i].amount;

        if (do_wydania >= nominal)
        {
            int potrzebne = do_wydania / nominal;
            int wezmiemy = (potrzebne <= dostepne_sztuki) ? potrzebne : dostepne_sztuki;

            if (isWithdrawal == 1)
            {
                atm_money[i].amount -= wezmiemy;
            }
            else if (isWithdrawal == 0)
            {
                atm_money[i].amount += wezmiemy;
            }
            do_wydania -= (wezmiemy * nominal);
        }
    }
}

int check_can_withdraw(int kwota)
{
    int do_wydania = kwota;
    for (int i = atm_count - 1; i >= 0; i--)
    {
        int nominal = atm_money[i].value;
        int dostepne_sztuki = atm_money[i].amount;

        if (do_wydania >= nominal)
        {
            int potrzebne = do_wydania / nominal;
            int wezmiemy = (potrzebne <= dostepne_sztuki) ? potrzebne : dostepne_sztuki;
            do_wydania -= (wezmiemy * nominal);
        }
    }
    return (do_wydania == 0);
}

void handle_deposit(int index)
{
    double wplata;
    printf("Podaj kwote do wplaty (max 2000, wielokrotnosc 10): ");
    if (scanf("%lf", &wplata) != 1) {
        while (getchar() != '\n');
        return;
    }
    if (wplata > 0 && wplata <= 2000 && (fmod(wplata, 10.0) == 0)) {
        update_atm_cash((int)wplata, 0);
        tab[index].balance += wplata;
        printf("Sukces! Nowe saldo: %.2f\n", tab[index].balance);
        char msg[100];
        sprintf(msg, "Wplata: +%.2f", wplata);
        log_activity(tab[index].num, msg);
    } else {
        printf("Blad: Nieprawidlowa kwota lub bankomat nie przyjmuje takich nominalow.\n");
    }
}

void handle_withdrawal(int index)
{
    double wyplata;
    printf("Podaj kwote do wyplaty (wielokrotnosc 10): ");
    if (scanf("%lf", &wyplata) != 1) {
        while (getchar() != '\n');
        return;
    }
    if (wyplata > 0 && wyplata <= tab[index].balance && (fmod(wyplata, 10.0) == 0)) {
        if (check_can_withdraw((int)wyplata)) {
            update_atm_cash((int)wyplata, 1);
            tab[index].balance -= wyplata;
            printf("Wyplacono: %.2f. Nowe saldo: %.2f\n", wyplata, tab[index].balance);
            char msg[100];
            sprintf(msg, "Wyplata: -%.2f", wyplata);
            log_activity(tab[index].num, msg);
        } else {
            printf("Bankomat nie posiada odpowiednich banknotow do wydania tej kwoty.\n");
        }
    } else {
        printf("Blad: Nieprawidlowa kwota lub brak srodkow.\n");
    }
}