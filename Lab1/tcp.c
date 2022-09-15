#include "tcp.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
void tcp_server_create(struct tcp_server_t *server, int port)
{
    /********** Bindear socket a puerto *******************/
    memset(&server->server_addr, 0, sizeof(server->server_addr));
    server->server_addr.sin_family = AF_INET;
    server->server_addr.sin_addr.s_addr = INADDR_ANY;
    server->server_addr.sin_port = htons(port);
    server->listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Crear socket de escucha (se guarda en server->listen_sock)
    if (server->listen_sock < 0)
    {
        fprintf(stderr, "could not create server socket\n");
        exit(1);
    }
    int enable = 1;

    if (setsockopt(server->listen_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
        printf("could not configure socket");
        exit(1);
    }

    int b = bind(server->listen_sock, (struct sockaddr *)&server->server_addr, sizeof(server->server_addr));
    if (b < 0)
    {
        close(server->listen_sock);
        fprintf(stderr, "could not bind socket\n");
        server->listen_sock = -1;
        exit(1);
    }

    /****************** Escuchar conexiones entrantes *******************/
    if (listen(server->listen_sock, 5) < 0)
    {
        fprintf(stderr, "could not listen to connections\n");
        exit(1);
    }
}

int tcp_server_accept(struct tcp_server_t *server,
                      struct sockaddr_in *client_addr,
                      socklen_t *client_addr_len)
{
    int sock;
    // Aceptar primera conexion entrante (guardar dirección del cliente en
    // client_addr)
    int lenght = sizeof(struct sockaddr_in);
    if ((sock =
             accept(server->listen_sock, (struct sockaddr *)&client_addr, client_addr_len)) < 0)
    {
        fprintf(stderr, "the connection is not accepted\n");
    }
    // Retorna descriptor de archivo del socket de comunicación con el cliente
    return sock;
}

void tcp_client_connect(struct tcp_client_t *client, const char *host,
                        int port)
{
    // Crear socket de cliente (se guarda en client->sock)
    client->sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client->sock < 0)
    {
        fprintf(stderr, "could not create client socket\n");
        exit(1);
    }
    memset(&client->server_addr, 0, sizeof(client->server_addr));
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port); // Pasar host a entero;

    // Se debe ob
    struct addrinfo *res;
    /* nodo, servicio , sugerencias ,  */
    int x = getaddrinfo(host, NULL, NULL, &res);
    if (x != 0)
    {                        // 0 si tiene algun error
        close(client->sock); // se cierra el socket del cliente
        client->sock = -1;
        fprintf(stderr, "could not save client\n");
        return;
    }
    // Conectar con host y puerto indicados (se guarda en client->server_addr y
    // se usa en llamada a connect())
    if (connect(client->sock, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr)) < 0)
    {
        fprintf(stderr, "could not connect\n");
        close(client->sock);
        client->sock = -1;
        exit(1);
    }
}

void tcp_send(int sock, const void *data, size_t size)
{
    long Bytes = size;
    long Faltan = 0;
    int Porcentaje = 0;
    int bloque = 500;
    long i = 0;
    Faltan = Bytes;
    while (i < Bytes)
    {
        if (Faltan < bloque)
        {
            bloque = Faltan;
        }
        size_t t = send(sock, data + i, bloque, 0);
        i += t;
        Faltan -= bloque;
        Porcentaje = (i * 100) / Bytes;
        if (Porcentaje < 100)
            printf("Carga : %d\n", Porcentaje); // printf("Falta = %li\n",Faltan);
    }
    printf("Carga : 100\n");
}

void tcp_recv(int sock, void *data, size_t size)
{

    size_t remaining = size;
    while (remaining > 0)
    {
        size_t n = recv(sock, data, remaining, 0);
        data += n;
        remaining -= n;
        int Porcentaje = 100 - ((remaining * 100) / size);
        printf("Carga : %d\n", Porcentaje);
    }
}

void tcp_close(int sock)
{
    close(sock);
}
