#include <math.h>
#include <stdio.h>
#include <unistd.h>

#include "globals.h"
#include "cash_manager.h"

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

// Zwraca 1, jeśli bankomat może wydać taką kwotę.
// Zwraca 0, jeśli brakuje odpowiednich nominałów.
int check_can_withdraw(int kwota)
{
    int do_wydania = kwota;

    // iterujemy od NAJWIĘKSZEGO nominału do najmniejszego
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

    if (do_wydania == 0)
        return 1;
    else
        return 0;
}

void handle_deposit(int index)
{
    double wplata;
    printf("Podaj kwotę do wpłaty (max 2000): ");

    alarm(30);

    if (scanf("%lf", &wplata) != 1)
    {
        printf("Błąd: To nie jest poprawna liczba!\n");
        while (getchar() != '\n')
            ;
        alarm(0);
        return;
    }
    alarm(0);

    if (wplata <= 0)
    {
        printf("Błąd: Kwota musi być większa od zera!\n");
    }
    else if (wplata > 2000)
    {
        printf("Błąd: Maksymalna kwota jednorazowej wpłaty to 2000!\n");
    }
    else if (!(fabs(fmod(wplata, 10.0)) < 1e-9))
    {
        printf("Bankomat dysponuje tylko banknotami.\n");
    }
    else
    {
        update_atm_cash(wplata, 0);
        tab[index].balance += wplata;
        printf("Sukces! Wpłacono %.2f. Nowe saldo: %.2f\n",
               wplata, tab[index].balance);
    }
}

void handle_withdrawal(int index)
{
    double wyplata;
    printf("Podaj kwotę do wypłaty (max 2000): ");

    alarm(30);

    if (scanf("%lf", &wyplata) != 1)
    {
        printf("Błąd: To nie jest poprawna liczba!\n");
        // czyszczenie bufora
        while (getchar() != '\n')
            ;
        alarm(0);
        return;
    }

    alarm(0);

    if (wyplata <= 0)
    {
        printf("Błąd: Kwota musi być większa od zera!\n");
    }
    else if (wyplata > 2000)
    {
        printf("Błąd: Maksymalna kwota jednorazowej wypłaty to 2000!\n");
    }
    else if (wyplata > tab[index].balance)
    {
        printf("Brak wystarczających środków!\n");
    }
    else if (!(fabs(fmod(wyplata, 10.0)) < 1e-9))
    {
        printf("Bankomat dysponuje tylko banknotami.\n");
    }
    else if (!check_can_withdraw(wyplata))
    {
        printf("Bankomat nie posiada obecnie wystarczającej liczby środków.\n");
    }
    else
    {
        update_atm_cash(wyplata, 1);
        tab[index].balance -= wyplata;
        printf("Wypłacono %.2f. Nowe saldo: %.2f\n",
               wyplata, tab[index].balance);
    }
}