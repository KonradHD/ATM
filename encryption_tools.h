#ifndef ENCRYPTION_TOOLS_H
#define ENCRYPTION_TOOLS_H

int aes_encrypt(const unsigned char *, int, const unsigned char *, const unsigned char *, unsigned char *);
void print_hex(const char *, const unsigned char *, int);
int load_key_iv_from_file(const char *, unsigned char *, unsigned char *);
void generate_key_file(const char *);
int aes_decrypt(const unsigned char *, int, const unsigned char *, const unsigned char *, unsigned char *);

#endif