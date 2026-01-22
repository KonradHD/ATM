#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#include "globals.h"
#include "encryption_tools.h"

/**
 * Funkcja szyfrująca tekst algorytmem AES-256-CBC.
 * * @param plaintext     Wskaźnik na tekst do zaszyfrowania.
 * @param plaintext_len Długość tekstu.
 * @param key           Klucz (musi mieć 32 bajty dla AES-256).
 * @param iv            Wektor inicjalizujący (musi mieć 16 bajtów).
 * @param ciphertext    Bufor wyjściowy (musi być co najmniej plaintext_len + 16 bajtów).
 * @return              Długość zaszyfrowanego tekstu lub -1 w przypadku błędu.
 */
int aes_encrypt(const unsigned char *plaintext, int plaintext_len,
                const unsigned char *key, const unsigned char *iv,
                unsigned char *ciphertext)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    // 1. Utwórz i zainicjuj kontekst
    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        return -1;
    }

    // 2. Inicjalizacja operacji szyfrowania.
    // Używamy EVP_aes_256_cbc(). Dla kluczy 128-bit zmień na EVP_aes_128_cbc().
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    // 3. Dostarcz wiadomość do zaszyfrowania (może być wywoływane wielokrotnie dla strumieni)
    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    ciphertext_len = len;

    // 4. Finalizacja szyfrowania (dodanie paddingu PKCS#7)
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    ciphertext_len += len;

    // 5. Czyszczenie pamięci
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

// Funkcja pomocnicza do wyświetlania danych binarnych jako HEX
void print_hex(const char *label, const unsigned char *data, int len)
{
    printf("%s: ", label);
    for (int i = 0; i < len; i++)
    {
        printf("%02x", data[i]);
    }
    printf("\n");
}

int load_key_iv_from_file(const char *filename, unsigned char *key, unsigned char *iv)
{
    FILE *file = fopen(filename, "rb"); // Otwarcie w trybie binarnym (ważne!)
    if (!file)
    {
        perror("Nie można otworzyć pliku klucza");
        return -1;
    }

    // Odczyt klucza
    if (fread(key, 1, KEY_SIZE, file) != KEY_SIZE)
    {
        fprintf(stderr, "Błąd: Plik jest za krótki, nie można wczytać klucza.\n");
        fclose(file);
        return -1;
    }

    // Odczyt IV
    if (fread(iv, 1, IV_SIZE, file) != IV_SIZE)
    {
        fprintf(stderr, "Błąd: Plik jest za krótki, nie można wczytać IV.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

/**
 * Funkcja pomocnicza do wygenerowania przykładowego pliku z kluczem i IV.
 * W normalnym użyciu taki plik byłby generowany i przechowywany bezpiecznie.
 */
void generate_key_file(const char *filename)
{
    unsigned char key[KEY_SIZE];
    unsigned char iv[IV_SIZE];

    // Generowanie losowych danych kryptograficznych
    RAND_bytes(key, KEY_SIZE);
    RAND_bytes(iv, IV_SIZE);

    FILE *file = fopen(filename, "wb");
    if (file)
    {
        fwrite(key, 1, KEY_SIZE, file);
        fwrite(iv, 1, IV_SIZE, file);
        fclose(file);
        printf("Wygenerowano nowy plik klucza: %s\n", filename);
    }
    else
    {
        perror("Błąd zapisu pliku klucza");
    }
}

/**
 * Odszyfrowuje dane algorytmem AES-256-CBC
 */
int aes_decrypt(const unsigned char *ciphertext, int ciphertext_len,
                const unsigned char *key, const unsigned char *iv,
                unsigned char *plaintext)
{

    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new()))
        return -1;

    // WAŻNE: Używamy EVP_DecryptInit_ex zamiast EncryptInit
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
    {
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
    {
        // Błąd tutaj oznacza zazwyczaj złe hasło lub uszkodzone dane
        EVP_CIPHER_CTX_free(ctx);
        return -1;
    }
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}