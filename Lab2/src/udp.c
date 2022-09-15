#include "udp.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void resolve_host(struct addrinfo *result, const char *host)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (getaddrinfo(host, NULL, &hints, &result) != 0)
    {
        fprintf(stderr, "could not resolve host\n");
        exit(1);
    }
    printf("%s", host);
}

void udp_server_init(struct udp_socket_t *server, int port)
{
    if ((server->sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "could not create socket: %s\n", strerror(errno));
        exit(1);
    }

    memset(&server->address, 0, sizeof(server->address));
    server->address.address.sin_family = AF_INET;
    server->address.address.sin_port = htons(port);
    server->address.address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server->sock, (struct sockaddr *)&server->address,
             sizeof(server->address.address)) < 0)
    {
        fprintf(stderr, "could not bind socket: %s\n", strerror(errno));
        exit(1);
    }
}

void udp_client_init(struct udp_socket_t *client, const char *host, int port)
{
    if ((client->sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        fprintf(stderr, "could not create socket: %s\n", strerror(errno));
        exit(1);
    }

    client->address.address.sin_family = AF_INET;
    client->address.address.sin_port = htons(port);
    client->address.size = sizeof(struct sockaddr_in);

    if (inet_pton(AF_INET, host, &client->address.address.sin_addr.s_addr) <
        0)
    {
        fprintf(stderr, "invalid host address\n");
        exit(1);
    }
    printf("%s", host);
}

void udp_send(const struct udp_socket_t *socket, const struct addr_t *dest,
              const void *data, size_t size)
{
    if (sendto(socket->sock, data, size, 0, (struct sockaddr *)&dest->address,
               sizeof(dest->address)) < 0)
    {

        fprintf(stderr, "couldn't send datagram: %s\n", strerror(errno));
        exit(1);
    }
}

ssize_t udp_recv(const struct udp_socket_t *socket, struct addr_t *sender_addr,
                 void *buf, size_t max_size)
{
    struct sockaddr *addr = NULL;
    socklen_t *addr_len = NULL;

    if (sender_addr != NULL)
    {
        addr = (struct sockaddr *)&sender_addr->address;
        addr_len = &sender_addr->size;
    }

    return recvfrom(socket->sock, buf, max_size, 0, addr, addr_len);
}
