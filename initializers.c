#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "encryption_tools.h"
#include "validations.h"

#define SECRET_KEY "PRIVATE_KEY"

void xor_cipher(char *data, size_t length)
{
    size_t key_len = strlen(SECRET_KEY);
    for (size_t i = 0; i < length; i++)
    {
        // XOR
        data[i] = data[i] ^ SECRET_KEY[i % key_len];
    }
}

void init_accounts()
{
    tab = malloc(sizeof(Account));
    int capacity = 1;

    FILE *f = fopen("data/accounts.txt", "r");
    if (!f)
    {
        perror("Nie mozna otworzyc pliku");
        free(tab);
        return;
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, f) != -1)
    {
        if (licznik >= capacity)
        {
            capacity *= 2;
            tab = realloc(tab, capacity * sizeof(Account));
        }

        if (sscanf(line, "%29s %9s %lf", tab[licznik].num,
                   tab[licznik].pin, &tab[licznik].balance) == 3)
        {

            if (check_luhn(tab[licznik].num, 0) < 0)
            {
                printf("Błędny numer karty z wczytanych danych (Luhn fail): %s\n", tab[licznik].num);
                tab[licznik].isBlocked = 1;
                continue;
                ; // Przejdź do następnej linii w pliku, nie zwiększaj licznika
            }

            // Sprawdzenie poprawności PINu
            if (validate_pin(tab[licznik].pin, licznik) < 0)
            {
                printf("Błędny PIN (Format fail) dla karty %s\n", tab[licznik].num);
                tab[licznik].isBlocked = 1;
                continue; // Przejdź do następnej linii, nie zwiększaj licznika
            }

            tab[licznik].isBlocked = 0;
            licznik++;
        }
    }

    printf("Wczytano %d kont.", licznik);
    free(line);
    fclose(f);
}

void init_atm_money()
{
    FILE *f = fopen("data/cash.txt", "r");

    if (!f)
    {
        printf("Brak pliku cash.txt - tworzenie bazy domyslnej...\n");
        return;
    }

    int atm_capacity = 4;
    atm_money = malloc(atm_capacity * sizeof(Cash));
    atm_count = 0;

    char buffer[100];

    while (fgets(buffer, sizeof(buffer), f))
    {
        if (atm_count >= atm_capacity)
        {
            atm_capacity *= 2;
            Cash *temp = realloc(atm_money, atm_capacity * sizeof(Cash));
            if (!temp)
            {
                perror("Blad alokacji pamieci!");
                fclose(f);
                return;
            }
            atm_money = temp;
        }

        int val, amt;
        if (sscanf(buffer, "%d - %d", &val, &amt) == 2)
        {
            atm_money[atm_count].value = val;
            atm_money[atm_count].amount = amt;
            atm_count++;
        }
    }

    fclose(f);
    printf("Wczytano %d rodzajow banknotow do bankomatu.\n", atm_count);
}

void init_accounts_encrypted()
{
    FILE *f = fopen("data/accounts.dat", "rb");
    if (!f)
        return;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);

    if (file_size > 0)
    {
        char *buffer = malloc(file_size + 1);
        fread(buffer, 1, file_size, f);
        buffer[file_size] = '\0';
        fclose(f);

        xor_cipher(buffer, file_size);

        char *cursor = buffer;
        int offset;

        int capacity = 1;
        tab = malloc(sizeof(Account));

        while (sscanf(cursor, "%29s %9s %lf%n",
                      tab[licznik].num,
                      tab[licznik].pin,
                      &tab[licznik].balance,
                      &offset) == 3)
        {
            tab[licznik].isBlocked = 0;
            cursor += offset;
            licznik++;

            if (licznik >= capacity)
            {
                capacity *= 2;
                tab = realloc(tab, capacity * sizeof(Account));
            }
        }
        free(buffer);
    }
    else
    {
        fclose(f);
    }
}

void init_accounts_aes()
{
    // 1. Wczytaj Klucz i IV
    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];

    if (load_key_iv_from_file(DEFAULT_KEY_PATH, key, iv) != 0)
    {
        fprintf(stderr, "Błąd: Nie można wczytać kluczy szyfrujących.\n");
        return;
    }

    // 2. Otwórz plik z danymi (zaszyfrowany)
    FILE *f = fopen(ACCOUNTS_FILE_PATH, "rb"); // Tryb binarny jest kluczowy!
    if (!f)
        return;

    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    rewind(f);

    if (file_size > 0)
    {
        // Alokacja pamięci na dane ZASZYFROWANE
        unsigned char *encrypted_buffer = malloc(file_size);
        fread(encrypted_buffer, 1, file_size, f);
        fclose(f);

        // Alokacja pamięci na dane ODSZYFROWANE (tekst jawny)
        // Rozmiar będzie taki sam lub mniejszy (padding), +1 na null terminator dla stringa
        unsigned char *decrypted_buffer = malloc(file_size + 1);

        // 3. Deszyfrowanie
        int decrypted_len = aes_decrypt(encrypted_buffer, file_size, key, iv, decrypted_buffer);

        if (decrypted_len == -1)
        {
            fprintf(stderr, "Błąd deszyfrowania danych (zły klucz lub uszkodzony plik).\n");
            free(encrypted_buffer);
            free(decrypted_buffer);
            return;
        }

        // Dodajemy znak końca stringa, aby sscanf działał poprawnie
        decrypted_buffer[decrypted_len] = '\0';

        // Już nie potrzebujemy zaszyfrowanych danych
        free(encrypted_buffer);

        // 4. Parsowanie (SSCANF na odszyfrowanym buforze)
        char *cursor = (char *)decrypted_buffer;
        int offset;
        int capacity = 1;

        // Jeśli tab nie jest jeszcze zainicjowana
        if (tab == NULL)
        {
            tab = malloc(sizeof(Account));
        }

        while (sscanf(cursor, "%29s %9s %lf%n",
                      tab[licznik].num,
                      tab[licznik].pin,
                      &tab[licznik].balance,
                      &offset) == 3)
        {
            tab[licznik].isBlocked = 0;
            cursor += offset;
            licznik++;

            if (licznik >= capacity)
            {
                capacity *= 2;
                // Ważne: realloc może zmienić adres pamięci, trzeba przypisać wynik
                Account *temp = realloc(tab, capacity * sizeof(Account));
                if (temp)
                {
                    tab = temp;
                }
                else
                {
                    // Obsługa błędu braku pamięci
                    fprintf(stderr, "Błąd alokacji pamięci!\n");
                    break;
                }
            }
        }

        free(decrypted_buffer);
    }
    else
    {
        fclose(f);
    }
}