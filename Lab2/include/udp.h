#ifndef UDP_H
#define UDP_H

#include <arpa/inet.h>
#include <netinet/in.h>

// Para solicitudes del cliente
struct request_pkt
{
    int Filenum;     // numero del archivo
    int type;        // 1: listado, 0: pedir bloque
    size_t block_no; // bloque a pedir
};
// Para enviar un bloque al cliente
struct data_pkt
{
    size_t block_no; // Bloque a pedir
    int block_size;  // tamaño
    char data[512];  // datos
};
// Para enviar listado de archivos al cliente
struct listing_pkt
{
    int files_no; // numero de archivos
    struct
    {
        char name[64]; // nombre
        size_t size;   // tamaño
    } files[10];       // max 10 archivos
};

struct addr_t
{
    struct sockaddr_in address;
    socklen_t size;
};

struct udp_socket_t
{
    struct addr_t address;
    int sock;
};

void udp_server_init(struct udp_socket_t *server, int port);
void udp_client_init(struct udp_socket_t *client, const char *host, int port);

void udp_send(const struct udp_socket_t *socket, const struct addr_t *dest,
              const void *data, size_t size);
ssize_t udp_recv(const struct udp_socket_t *socket, struct addr_t *sender_addr,
                 void *buf, size_t max_size);

#endif // UDP_H
