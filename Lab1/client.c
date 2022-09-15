#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "crypto.h"
#include "util.h"
#include "tcp.h"
#include <errno.h>

int main(int argc, char *argv[])
{
    struct tcp_client_t server;
    tcp_client_connect(&server, "localhost", 223333);
    const char *key_file = argv[1];

    unsigned char key[crypto_secretbox_KEYBYTES];
    open_key(key_file, key);
    struct encrypted_t enc;

    struct SizeName B;
    recv(server.sock, (struct SizeName *)&B, sizeof(B), 0);

    enc.msg = malloc(B.size2);

    tcp_recv(server.sock, enc.nonce, sizeof enc.nonce);
    tcp_recv(server.sock, enc.msg, B.size2);
    enc.len = B.size2;
    unsigned char *out = malloc(B.size2);
    if (decrypt(key, out, &enc) != 0)
    {
        fprintf(stderr, "%s: could not decrypt\n", argv[0]);
        return 1;
    }
    FILE *f_out;
    if ((f_out = fopen(B.Name, "wb")) == NULL)
    {
        fprintf(stderr, "%s: could not open output file\n", argv[0]);
        return 1;
    }
    const size_t data_len = B.size2 - crypto_secretbox_MACBYTES;
    write_file(f_out, out, data_len);
    tcp_close(server.sock);

    free(enc.msg);

    return 0;
}
