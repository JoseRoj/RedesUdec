#ifndef RUDP_H
#define RUDP_H

#include <stdint.h>

#include "udp.h"

struct rudp_header {
    uint32_t seq;
    uint32_t ts;
};

void rudp_server_recv(const struct udp_socket_t* socket, struct addr_t* from,
                      void* pkt, size_t size);
void rudp_server_send(const struct udp_socket_t* socket, struct addr_t* dest,
                      void* pkt, size_t size);
void rudp_client_send_recv(const struct udp_socket_t* socket,
                           struct addr_t* dest, void* send_pkt,
                           size_t send_size, void* recv_pkt, size_t recv_size);

#endif  // RUDP_H
