#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "globals.h"
#include "file_manager.h"
#include "initializers.h"
#include "encryption_tools.h"

void save_accounts_encrypted()
{
    FILE *f = fopen("data/accounts.dat", "wb");
    if (!f)
        return;

    size_t estimated_size = licznik * 150 + 1;
    char *buffer = malloc(estimated_size);
    if (!buffer)
    {
        fclose(f);
        return;
    }
    buffer[0] = '\0';

    char line[100];
    for (int i = 0; i < licznik; i++)
    {
        sprintf(line, "%s %s %.2f\n", tab[i].num, tab[i].pin, tab[i].balance);
        strcat(buffer, line);
    }

    size_t data_len = strlen(buffer);
    xor_cipher(buffer, data_len);
    fwrite(buffer, 1, data_len, f);

    fclose(f);
    free(buffer);
}

void save_accounts_aes()
{
    // 1. Wczytanie klucza i IV (identycznie jak przy odczycie)
    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];

    if (load_key_iv_from_file(DEFAULT_KEY_PATH, key, iv) != 0)
    {
        fprintf(stderr, "Błąd: Nie można wczytać klucza. Zapis anulowany.\n");
        return;
    }

    // 2. Przygotowanie bufora na tekst jawny (Plaintext)
    // Szacujemy rozmiar: np. 150 bajtów na jedno konto
    size_t estimated_size = (licznik * 150) + 1;
    char *plaintext_buffer = malloc(estimated_size);
    if (!plaintext_buffer)
    {
        fprintf(stderr, "Błąd alokacji pamięci na tekst.\n");
        return;
    }

    // Budowanie stringa z danymi (używamy wskaźnika dla szybkości zamiast strcat)
    char *cursor = plaintext_buffer;
    for (int i = 0; i < licznik; i++)
    {
        // sprintf zwraca liczbę zapisanych znaków
        int written = sprintf(cursor, "%s %s %.2f\n", tab[i].num, tab[i].pin, tab[i].balance);
        cursor += written; // Przesuwamy wskaźnik
    }

    // Obliczamy faktyczną długość tekstu
    int plaintext_len = cursor - plaintext_buffer;

    // 3. Przygotowanie bufora na dane zaszyfrowane (Ciphertext)
    // Musi być większy o rozmiar bloku (16 bajtów) dla paddingu
    unsigned char *ciphertext_buffer = malloc(plaintext_len + 16);
    if (!ciphertext_buffer)
    {
        free(plaintext_buffer);
        fprintf(stderr, "Błąd alokacji pamięci na szyfrogram.\n");
        return;
    }

    // 4. Szyfrowanie (AES)
    int ciphertext_len = aes_encrypt((unsigned char *)plaintext_buffer, plaintext_len, key, iv, ciphertext_buffer);

    if (ciphertext_len == -1)
    {
        fprintf(stderr, "Błąd krytyczny podczas szyfrowania danych!\n");
        free(plaintext_buffer);
        free(ciphertext_buffer);
        return;
    }

    // 5. Zapis do pliku
    FILE *f = fopen(ACCOUNTS_FILE_PATH, "wb"); // WB - Write Binary
    if (f)
    {
        fwrite(ciphertext_buffer, 1, ciphertext_len, f);
        fclose(f);
        printf("Pomyślnie zapisano i zaszyfrowano %d kont.\n", licznik);
    }
    else
    {
        perror("Nie udało się otworzyć pliku do zapisu");
    }

    // 6. Sprzątanie
    free(plaintext_buffer);
    free(ciphertext_buffer);
}

void save_state()
{
    // --- BLOKOWANIE SYGNAŁÓW ---
    sigset_t mask, oldmask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    save_accounts_aes();

    FILE *cash_file = fopen("data/cash.txt", "w");
    if (cash_file)
    {
        for (int i = 0; i < atm_count; i++)
        {
            fprintf(cash_file, "%d - %d\n", atm_money[i].value, atm_money[i].amount);
        }
        fclose(cash_file);
    }

    // --- PRZYWRÓCENIE SYGNAŁÓW ---
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}