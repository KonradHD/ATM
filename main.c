#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <math.h>
#include <pthread.h>

#include "globals.h"
#include "save.h"
#include "signal_handlers.h"
#include "initializers.h"
#include "validations.h"
#include "cash_manager.h"

void get_hidden_input(char *buf, size_t size)
{
    struct termios oldt, newt;
    size_t i = 0;
    int ch;

    // pobranie obecnych ustawienia terminala
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // ICANON - wyłącza buforowanie linii (nie trzeba klikać Enter)
    // ECHO   - wyłącza wyświetlanie znaków na ekranie
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (i < size - 1)
    {

        ch = getchar();

        // Obsługa Backspace
        if (ch == 127 || ch == '\b')
        {
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        }

        else if (ch == '\n' || ch == '\r')
        {
            break;
        }

        else
        {
            buf[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }

    buf[i] = '\0';

    // przywracanie ustawień terminala
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    printf("\n");
}

void *unlock_account_thread(void *arg)
{
    int index = *((int *)arg);
    free(arg);
    sleep(30);

    tab[index].isBlocked = 0;

    return NULL;
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_timeout);

    init_atm_money();
    // init_accounts(); // dane wczytują się z pliku accounts.txt
    init_accounts_encrypted(); // dane wczytują się z pliku accounts.dat

    printf("Witaj w bankomacie.\n");

    char num_[30], pin_[10];
    int zalogowany_index = -1;

    char *line = NULL;
    size_t len = 0;

    while (zalogowany_index == -1)
    {
        int account_correctness = -1;
        while (account_correctness < 0)
        {
            printf("\nPodaj numer karty: ");
            if (getline(&line, &len, stdin))
            {
                line[strcspn(line, "\n")] = '\0';
                account_correctness = validate_card_number(line);
                switch (account_correctness)
                {
                case -1:
                    printf("Numer konta musi mieć długość 16 cyfr.\n");
                    break;

                case -2:
                    printf("Numer karty musi składać się z samych cyfr.\n");
                    break;

                case -3:
                    printf("Podany numer konta nie istnieje.\n");
                    break;
                }
            }
        }

        strncpy(num_, line, sizeof(num_) - 1);
        num_[sizeof(num_) - 1] = '\0';
        num_[strcspn(num_, "\n")] = '\0';
        int account_index = account_correctness;

        if (tab[account_index].isBlocked == 1)
        {
            printf("Podany numer konta jest zablokowany.\n");
            continue;
        }

        int pin_correctness = -1;
        int tries = 1;
        while (pin_correctness < 0 && tab[account_index].isBlocked == 0)
        {
            printf("Podaj PIN: ");
            get_hidden_input(pin_, sizeof(pin_));
            pin_correctness = validate_pin(pin_, account_index);

            switch (pin_correctness)
            {
            case -1:
                tries++;
                printf("PIN powinien mieć długość 4 cyfr.\n");
                break;

            case -2:
                tries++;
                printf("PIN powinien składać się z samych cyfr.\n");
                break;

            case -3:
                tries++;
                printf("Podany PIN nie jest poprawny.\n");
                break;

            case 0:
                printf("Podano poprawny PIN.\n");
                zalogowany_index = account_index;
                break;
            }

            if (tries > 3)
            {
                tab[account_index].isBlocked = 1;
                printf("Podany numer karty został zablokowany na czas 30 sekund z powodu przekroczenia liczby prób logowania.\n");

                int *arg = malloc(sizeof(*arg));
                if (arg == NULL)
                {
                    fprintf(stderr, "Blad pamieci!\n");
                    exit(1);
                }
                *arg = account_index;

                pthread_t tid;

                // uruchomienie wątku
                if (pthread_create(&tid, NULL, unlock_account_thread, arg) != 0)
                {
                    printf("Nie udalo sie uruchomic licznika blokady!\n");
                    free(arg);
                }

                pthread_detach(tid);
                break;
            }
        }
    }

    printf("Logowanie udane!\n");

    int choice;
    do
    {
        printf("\n--- MENU ---\n");
        printf("1. Sprawdź saldo\n");
        printf("2. Wpłata\n");
        printf("3. Wypłata\n");
        printf("4. Wylogowanie\n");
        printf("Wybierz opcję: ");

        alarm(30);

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ; // czyścimy bufor
            printf("Niepoprawna opcja!\n");
            continue;
        }

        alarm(0);

        switch (choice)
        {
        case 1:
            printf("Saldo: %.2f\n", tab[zalogowany_index].balance);
            break;
        case 2:
        {
            handle_deposit(zalogowany_index);
            break;
        }
        case 3:
        {
            handle_withdrawal(zalogowany_index);
            break;
        }
        case 4:
            printf("Do widzenia!\n");
            break;
        default:
            printf("Niepoprawna opcja!\n");
        }
    } while (choice != 4);

    save_state();

    free(tab);
    free(atm_money);
    return 0;
}
