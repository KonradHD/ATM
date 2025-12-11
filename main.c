#include <stdio.h>
#include <string.h>

int main() {

    FILE *fp_test = fopen("accounts.txt", "w");
    if (fp_test) {
        fprintf(fp_test, "1234 25/25 1234 1234 5000.00\n");
        fprintf(fp_test, "9876 01/26 4321 999 100.50\n");
        fclose(fp_test);
        printf("--- PLIK accounts.txt ZOSTAL STWORZONY POPRAWNIE ---\n");
    }

    char nr_[30];
    char date_[20];
    char pin_[10];
    char cvv_[10];
    double balance_;

    printf("Witaj w bankomacie. Wprowadz dane swojej karty! \n");

    while (1) {

        printf("\nPodaj numer karty: ");
        fgets(nr_, 30, stdin);
        nr_[strcspn(nr_, "\n")] = 0;

        printf("Podaj date waznosci karty (MM/YY): ");
        fgets(date_, 20, stdin);
        date_[strcspn(date_, "\n")] = 0;

        printf("Podaj PIN: ");
        fgets(pin_, 10, stdin);
        pin_[strcspn(pin_, "\n")] = 0;

        printf("Podaj CVV: ");
        fgets(cvv_, 10, stdin);
        cvv_[strcspn(cvv_, "\n")] = 0;

        printf("Input: '%s' '%s' '%s' %s\n", nr_, date_, pin_, cvv_);;

        FILE *f = fopen("accounts.txt", "r");
        if (!f) {
            printf("Blad otwarcia pliku!\n");
            return 1;
        }

        char c1[30], c2[20], c3[10], c4[10];
        double d1;
        int flag = 0;

        while (fscanf(f, "%29s %19s %9s %9s %lf", c1, c2, c3, c4, &d1) == 5) {
            printf("Plik: '%s' '%s' '%s' %s\n", c1, c2, c3, c4);
            if (strcmp(c1, nr_) == 0 && strcmp(c2, date_) == 0 && strcmp(c3, pin_) == 0 && strcmp(c4, cvv_) == 0) {
                flag = 1;
                balance_ = d1;
                break;
            }
        }

        fclose(f);

        if (flag) {
            printf("Dane poprawne! Dostęp przyznany.\n");
            break;
        }
        if (!flag) {
            printf("Bledne dane karty! Sprobuj ponownie.\n");
        }
    }

    while (1){
        int wybor;

        printf("\n===== BANKOMAT =====\n");
        printf("1. Sprawdz saldo\n");
        printf("2. Wyplata gotowki\n");
        printf("3. Wplata gotowki\n");
        printf("4. Wyjscie\n");
        printf("Wybierz opcje: ");
        scanf("%d", &wybor);

        if (wybor == 1) {
            printf("Saldo: %.2lf", balance_);
        }
        else if (wybor == 2) {
            while (1) {
                int ile;
                printf("Ile gotówki chcesz wypłacić?");
                scanf("%d", &ile);
                if (ile <= balance_) {
                    balance_ -= ile;
                    printf("Wypłata udana. Nowy stan konta: %lf", balance_);
                    break;
                }
                else {
                    printf("Nie masz tyle środków na koncie!");
                }
            }
        }
        else if (wybor == 3) {
            int ile;
            printf("Ile gotówki chcesz wpłacić?");
            scanf("%d", &ile);
            balance_ += ile;
            printf("Wpłata udana. Nowy stan konta: %lf", balance_);
        }
        else if (wybor == 4) {
            printf("Dziękujemy za skorzystanie z naszych usług");
            break;
        }
        else {
            printf("Nieznana opcja! Podaj opcję z podanych");
        }
    }

    return 0;
}
