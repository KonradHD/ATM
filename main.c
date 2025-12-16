#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

typedef struct {
    char num[30];
    char pin[10];
    double balance;
} Account;

void get_hidden_input(char *buf, size_t size) {
    size_t i = 0;
    char ch;
    while (i < size - 1) {
        ch = _getch();
        if (ch == '\r') break;
        if (ch == '\b' && i > 0) {
            i--;
            printf("\b \b");
        } else {
            buf[i++] = ch;
            printf("*");
        }
    }
    buf[i] = '\0';
    printf("\n");
}

int main(void) {
    Account *tab = malloc(sizeof(Account));
    int licznik = 0;
    int capacity = 1;

    FILE *f = fopen("accounts.txt", "r");
    if (!f) {
        perror("Nie mozna otworzyc pliku");
        free(tab);
        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, f) != -1) {
        if (licznik >= capacity) {
            capacity *= 2;
            tab = realloc(tab, capacity * sizeof(Account));
        }

        if (sscanf(line, "%29s %9s %lf", tab[licznik].num,
                   tab[licznik].pin, &tab[licznik].balance) == 3) {
            licznik++;
        }
    }

    free(line);
    fclose(f);

    printf("Witaj w bankomacie.\n");

    char num_[30], pin_[10];
    int zalogowany_index = -1;

    while (zalogowany_index == -1) {
        printf("\nPodaj numer karty: ");
        if (getline(&line, &len, stdin)) {
            strncpy(num_, line, sizeof(num_)-1);
            num_[sizeof(num_)-1] = '\0';
            num_[strcspn(num_, "\n")] = '\0';
        }

        printf("Podaj PIN: ");
        get_hidden_input(pin_, sizeof(pin_));

        for (int i = 0; i < licznik; i++) {
            if (strcmp(tab[i].num, num_) == 0 &&
                strcmp(tab[i].pin, pin_) == 0) {
                zalogowany_index = i;
                break;
            }
        }

        if (zalogowany_index == -1) {
            printf("Błędne dane karty! Spróbuj ponownie.\n");
        }
    }

    printf("Logowanie udane!\n");

    int choice;
    do {
        printf("\n--- MENU ---\n");
        printf("1. Sprawdź saldo\n");
        printf("2. Wpłata\n");
        printf("3. Wypłata\n");
        printf("4. Wyjście\n");
        printf("Wybierz opcję: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // czyścimy bufor
            printf("Niepoprawna opcja!\n");
            continue;
        }

        switch (choice) {
            case 1:
                printf("Saldo: %.2f\n", tab[zalogowany_index].balance);
                break;
            case 2: {
                double wplata;
                printf("Podaj kwotę do wpłaty: ");
                scanf("%lf", &wplata);
                tab[zalogowany_index].balance += wplata;
                printf("Wpłacono %.2f. Nowe saldo: %.2f\n",
                       wplata, tab[zalogowany_index].balance);
                break;
            }
            case 3: {
                double wyplata;
                printf("Podaj kwotę do wypłaty: ");
                scanf("%lf", &wyplata);
                if (wyplata > tab[zalogowany_index].balance) {
                    printf("Brak wystarczających środków!\n");
                } else {
                    tab[zalogowany_index].balance -= wyplata;
                    printf("Wypłacono %.2f. Nowe saldo: %.2f\n",
                           wyplata, tab[zalogowany_index].balance);
                }
                break;
            }
            case 4:
                printf("Do widzenia!\n");
                break;
            default:
                printf("Niepoprawna opcja!\n");
        }
    } while (choice != 4);

    f = fopen("accounts.txt", "w");
    if (f) {
        for (int i = 0; i < licznik; i++) {
            fprintf(f, "%s %s %.2f\n",
                    tab[i].num, tab[i].pin, tab[i].balance);
        }
        fclose(f);
    }

    free(tab);
    return 0;
}
