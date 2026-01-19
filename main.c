#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <termio.h>

#include "globals.h"
#include "file_manager.h"
#include "signal_handlers.h"
#include "initializers.h"
#include "validations.h"
#include "cash_manager.h"
#include "history.h"

void get_hidden_input(char *buf, size_t size)
{
#ifdef _WIN32
    size_t i = 0;
    int ch;
    while (i < size - 1)
    {
        ch = _getch();
        if (ch == 13 || ch == 10)
            break;
        if (ch == 8)
        { // Backspace
            if (i > 0)
            {
                i--;
                printf("\b \b");
                fflush(stdout);
            }
        }
        else
        {
            buf[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
    buf[i] = '\0';
    printf("\n");
#else
    struct termios oldt, newt;
    size_t i = 0;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (i < size - 1)
    {
        ch = getchar();
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
            break;
        else
        {
            buf[i++] = ch;
            printf("*");
            fflush(stdout);
        }
    }
    buf[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
#endif
}

void *unlock_account_thread(void *arg)
{
    if (arg == NULL)
        return NULL;
    int index = *((int *)arg);
    free(arg);
    sleep(30);
    if (tab != NULL)
    {
        tab[index].isBlocked = 0;
    }
    return NULL;
}

int main(void)
{
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_timeout);

    init_atm_money();
    // init_accounts();
    init_accounts_aes();

    while (1)
    { // GŁÓWNA PĘTLA PROGRAMU
        printf("\n=== WITAJ W BANKOMACIE ===\n");
        char pin_[10];
        int zalogowany_index = -1;
        char *line = NULL;
        size_t len = 0;

        // Logowanie
        while (zalogowany_index == -1)
        {
            printf("\nPodaj numer karty (16 cyfr) lub CTRL+C aby wyjsc: ");
            if (getline(&line, &len, stdin) == -1)
                exit(0);
            line[strcspn(line, "\n")] = '\0';

            int account_index = check_luhn(line, 1);
            if (account_index < 0)
            {
                printf("Bledny numer karty.\n");
                continue;
            }

            if (tab[account_index].isBlocked)
            {
                printf("Konto zablokowane. Sprobuj pozniej.\n");
                continue;
            }

            int tries = 0;
            while (tries < 3)
            {
                printf("Podaj PIN: ");
                get_hidden_input(pin_, sizeof(pin_));
                if (validate_pin(pin_, account_index) == 0)
                {
                    zalogowany_index = account_index;
                    break;
                }
                tries++;
                printf("Bledny PIN. Proba %d/3\n", tries);
            }

            if (zalogowany_index == -1)
            {
                tab[account_index].isBlocked = 1;
                printf("Konto zablokowane na 30s.\n");
                int *arg = malloc(sizeof(int));
                *arg = account_index;
                pthread_t tid;
                pthread_create(&tid, NULL, unlock_account_thread, arg);
                pthread_detach(tid);
            }
        }

        log_activity(tab[zalogowany_index].num, "Zalogowano do systemu.");

        // Menu
        int choice = 0;
        while (choice != 4)
        {
            printf("\n1. Saldo 2. Wplata 3. Wyplata 4. Wyloguj: ");
            alarm(30);
            if (scanf("%d", &choice) != 1)
            {
                while (getchar() != '\n')
                    ;
                continue;
            }
            alarm(0);

            if (choice == 1)
                printf("Saldo: %.2f\n", tab[zalogowany_index].balance);
            else if (choice == 2)
                handle_deposit(zalogowany_index);
            else if (choice == 3)
                handle_withdrawal(zalogowany_index);
        }

        log_activity(tab[zalogowany_index].num, "Wylogowano.");
        save_state(); // Zapisujemy stan po każdym wylogowaniu
        printf("Wylogowano pomyslnie.\n");
        while (getchar() != '\n')
            ; // Czyszczenie bufora przed nowym logowaniem
    }

    return 0;
}