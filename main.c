#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

/* Obsługa różnic między Windows a Linux */
#ifdef _WIN32
    #include <conio.h>
    #define alarm(x) (0)
    #define SIGALRM 14
#else
    #include <termios.h>
#endif

#include "globals.h"
#include "save.h"
#include "signal_handlers.h"
#include "initializers.h"
#include "validations.h"
#include "cash_manager.h"
#include "history.h"

/* Funkcja do bezpiecznego wprowadzania PINu */
void get_hidden_input(char *buf, size_t size) {
#ifdef _WIN32
    size_t i = 0; int ch;
    while (i < size - 1) {
        ch = _getch();
        if (ch == 13 || ch == 10) break;
        if (ch == 8) { if (i > 0) { i--; printf("\b \b"); fflush(stdout); } }
        else { buf[i++] = ch; printf("*"); fflush(stdout); }
    }
    buf[i] = '\0'; printf("\n");
#else
    struct termios oldt, newt;
    size_t i = 0; int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while (i < size - 1) {
        ch = getchar();
        if (ch == 127 || ch == '\b') { if (i > 0) { i--; printf("\b \b"); fflush(stdout); } }
        else if (ch == '\n' || ch == '\r') break;
        else { buf[i++] = ch; printf("*"); fflush(stdout); }
    }
    buf[i] = '\0';
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\n");
#endif
}

/* Wątek odblokowujący konto po 30 sekundach */
void *unlock_account_thread(void *arg) {
    if (arg == NULL) return NULL;
    int index = *((int *)arg);
    free(arg);
    sleep(30);
    if (tab != NULL) {
        tab[index].isBlocked = 0;
    }
    return NULL;
}

int main(void) {
    /* Rejestracja sygnałów */
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_timeout);

    /* Inicjalizacja danych */
    init_atm_money();
    init_accounts_encrypted();

    /* Główna pętla programu - powrót do logowania po wylogowaniu */
    while (1) {
        current_user_index = -1; // Nikt nie jest zalogowany
        printf("\n=== WITAJ W BANKOMACIE ===\n");

        char pin_[10];
        int zalogowany_index = -1;
        char buffer[100];

        /* Pętla logowania */
        while (zalogowany_index == -1) {
            printf("\nPodaj numer karty (16 cyfr): ");
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) exit(0);
            buffer[strcspn(buffer, "\n")] = '\0';

            int account_index = validate_card_number(buffer);
            if (account_index < 0) {
                switch(account_index) {
                    case -1: printf("Numer konta musi miec 16 cyfr.\n"); break;
                    case -2: printf("Numer karty musi skladac sie z cyfr.\n"); break;
                    case -3: printf("Podany numer nie istnieje.\n"); break;
                }
                continue;
            }

            if (tab[account_index].isBlocked) {
                printf("To konto jest obecnie zablokowane. Sprobuj pozniej.\n");
                continue;
            }

            int tries = 0;
            int pin_correct = 0;
            while (tries < 3) {
                printf("Podaj PIN: ");
                get_hidden_input(pin_, sizeof(pin_));

                if (validate_pin(pin_, account_index) == 0) {
                    zalogowany_index = account_index;
                    current_user_index = zalogowany_index; // Ustawienie dla sygnałów
                    pin_correct = 1;
                    break;
                }
                tries++;
                if (tries < 3) printf("Bledny PIN. Pozostalo prob: %d\n", 3 - tries);
            }

            if (!pin_correct) {
                tab[account_index].isBlocked = 1;
                printf("Konto zablokowane na 30 sekund.\n");

                int *arg = malloc(sizeof(int));
                *arg = account_index;
                pthread_t tid;
                pthread_create(&tid, NULL, unlock_account_thread, arg);
                pthread_detach(tid);
            }
        }

        printf("Zalogowano pomyslnie!\n");
        log_activity(tab[zalogowany_index].num, "Zalogowano do systemu.");

        /* Menu operacji */
        int choice = 0;
        do {
            printf("\n--- MENU ---\n");
            printf("1. Sprawdz saldo\n");
            printf("2. Wplata\n");
            printf("3. Wyplata\n");
            printf("4. Wyloguj\n");
            printf("Wybierz opcje: ");

            alarm(30); // Start timer
            if (scanf("%d", &choice) != 1) {
                while (getchar() != '\n');
                continue;
            }
            alarm(0); // Stop timer

            switch (choice) {
                case 1:
                    printf("Saldo: %.2f\n", tab[zalogowany_index].balance);
                    break;
                case 2:
                    handle_deposit(zalogowany_index);
                    break;
                case 3:
                    handle_withdrawal(zalogowany_index);
                    break;
                case 4:
                    log_activity(tab[zalogowany_index].num, "Wylogowano użytkownika.");
                    printf("Wylogowywanie...\n");
                    break;
                default:
                    printf("Niepoprawna opcja!\n");
            }
        } while (choice != 4);

        save_state();
        current_user_index = -1;
        while (getchar() != '\n'); // Czyszczenie bufora
    }

    return 0;
}