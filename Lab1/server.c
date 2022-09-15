#include <stdio.h>
#include <string.h>

#include "crypto.h"
#include <sodium.h>

#include "tcp.h"
#include "util.h"

int main(int argc, char *argv[])
{

    if (sodium_init() != 0)
    {
        fprintf(stderr, "%s: could not initialize cryptography\n", argv[0]);
        return 1;
    }

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s key_file in_file \n", argv[0]);
        return 1;
    }

    struct tcp_server_t server;
    struct SizeName A;

    tcp_server_create(&server, 223333);

    const char *key_file = argv[1]; // Clave
    const char *in_file = argv[2];  // Nombre del archivo
    unsigned char key[crypto_secretbox_KEYBYTES];
    open_key(key_file, key);

    FILE *archivo;
    archivo = fopen(in_file, "r");
    if (archivo == NULL)
    {
        fprintf(stderr, "%s: could not open input file\n", argv[0]);
        return 1;
    }
    strcpy(A.Name, in_file);
    size_t input_len = file_size(archivo);
    unsigned char *input_data = malloc(input_len);

    read_file(archivo, input_data, input_len);
    fclose(archivo);

    struct encrypted_t enc;
    encrypt(key, &enc, input_data, input_len);

    long Byteleidos = 0;
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int sock = tcp_server_accept(&server, &client_addr, &client_addr_len);
        printf("Connected\n");

        /************** ENVIAR NOMBRE Y TAMAÑO ********************/

        A.size1 = sizeof enc.nonce;
        A.size2 = enc.len;
        send(sock, (struct SizeName *)&A, sizeof(A), 0);

        /*********************************************************/

        printf("tamaño %li\n", enc.len);

        tcp_send(sock, enc.nonce, sizeof enc.nonce); // Se envian la 2 partes

        tcp_send(sock, enc.msg, enc.len);

        tcp_close(sock);
        break;
    }

    return 0;
}
