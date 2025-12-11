#include <stdio.h>
#include <string.h>

int main() {

    char nr_[30];
    char date_[10];
    int cvv_;
    double balance_;

    printf("Witaj w bankomacie. Wprowadz dane swojej karty! \n");

    while (1) {

        printf("\nPodaj numer karty: ");
        fgets(nr_, 30, stdin);
        nr_[strcspn(nr_, "\n")] = 0;    // usuń \n

        printf("Podaj date waznosci karty (MM/YY): ");
        fgets(date_, 10, stdin);
        date_[strcspn(date_, "\n")] = 0;

        printf("Podaj CVV: ");
        scanf("%d", &cvv_);
        getchar(); // czyścimy \n po scanf

        printf("\nNumer karty: %s\n", nr_);
        printf("Data ważności: %s\n", date_);
        printf("CVV: %d\n\n", cvv_);

        FILE *f = fopen("accounts.txt", "r");
        if (!f) {
            printf("Błąd otwarcia pliku!\n");
            return 1;
        }

        char c1[30], c2[10];
        int d1;
        double d2;
        int znaleziono = 0;

        while (fscanf(f, "%29[^,],%9[^,],%d,%lf\n", c1, c2, &d1, &d2) == 4) {

            if (strcmp(c1, nr_) == 0 &&
                strcmp(c2, date_) == 0 &&
                d1 == cvv_) {

                znaleziono = 1;
                break;
                }
        }

        fclose(f);

        if (znaleziono) {
            printf("Dane poprawne! Dostęp przyznany.\n");
            break;
        }
        if (!znaleziono) {
            printf("Błędne dane karty! Spróbuj ponownie.\n");
        }
    }

    while (1){
        int wybor;

        printf("\n===== BANKOMAT =====\n");
        printf("1. Sprawdź saldo\n");
        printf("2. Wypłata gotówki\n");
        printf("3. Wpłata gotówki\n");
        printf("4. Wyjście\n");
        printf("Wybierz opcję: ");
        scanf("%d", &wybor);

        if (wybor == 1) {
            printf("Saldo: ");
        }
        if (wybor == 2) {
            int ile;
            printf("Ile gotówki chcesz wypłacić?");
            scanf("%d", &ile);
            balance_ -= ile;
        }
        if (wybor == 3) {
            int ile;
            printf("Ile gotówki chcesz wpłacić?");
            scanf("%d", &ile);
            balance_ += ile;
        }
        if (wybor == 4) {
            printf("Dziękujemy za skorzystanie z naszych usług");
            break;
        }
        else {
            printf("Nieznana opcja!");
        }
    }

    return 0;
}
