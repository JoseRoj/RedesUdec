#include "rudp.h"

#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <unistd.h>

#include "rtt.h"

/* The msghdr structure is used to minimize the number of directly supplied parameters to
the recvmsg() and sendmsg(). This structure us used as a value - result parameter in the
recvmsg() function and value only for the send sendmsg() function
msg_name :contain the adress.
msg_namelen : contain adrees length to wich the is message is sent.
*/
static struct msghdr server_msg;

/* rud_header save two integers */
static struct rudp_header server_header;
/*
const struc udp_socket_t *socket : Server UDP.
struct addr_t *from : Save client IP.
void *pkt :  data to receive.
size_t size : size of data.
************************************************
The struct iovec defines one vector element.
For each transfer element, the pointer:
iov_base : points to a buffer that is receiving data for readv or is transmitting data for writev.
iov_len  : maximum receive length and the actual write length*/
void rudp_server_recv(const struct udp_socket_t *socket, struct addr_t *from,
                      void *pkt, size_t size)
{
    if (from != NULL) // Client not NULL
    {
        server_msg.msg_name = &from->address;
        server_msg.msg_namelen = sizeof(from->address);
    }

    struct iovec iov[2];

    server_msg.msg_iov = iov;
    server_msg.msg_iovlen = 2;
    iov[0].iov_base = &server_header;
    iov[0].iov_len = sizeof(struct rudp_header);
    iov[1].iov_base = pkt;
    iov[1].iov_len = size;
    /* recvmsg  reive a message from a conection-mode or connectionless-mode socket.
    permits the application to retrieve the source adress of received data.
     */
    if (recvmsg(socket->sock, &server_msg, 0) < 0)
    {
        fprintf(stderr, "couldn't receive packet: %s\n", strerror(errno));
        exit(1);
    }
}

void rudp_server_send(const struct udp_socket_t *socket, struct addr_t *dest,
                      void *pkt, size_t size)
{
    struct iovec iov[2];

    server_msg.msg_name = &dest->address;
    server_msg.msg_namelen = sizeof(dest->address);
    server_msg.msg_iov = iov;
    server_msg.msg_iovlen = 2;
    iov[0].iov_base = &server_header;
    iov[0].iov_len = sizeof(struct rudp_header);
    iov[1].iov_base = pkt;
    iov[1].iov_len = size;

    sendmsg(socket->sock, &server_msg, 0);
}

static sigjmp_buf jmpbuf;

static struct rtt_t rtt_info;
static int rttinit = 0;

static struct msghdr client_recv_msg, client_send_msg;
static struct rudp_header client_recv_header, client_send_header;

static void sig_alrm(int signo)
{
    siglongjmp(jmpbuf, 1);
}

void rudp_client_send_recv(const struct udp_socket_t *socket,
                           struct addr_t *dest, void *send_pkt,
                           size_t send_size, void *recv_pkt, size_t recv_size)
{
    if (rttinit == 0)
    {
        rtt_init(&rtt_info);
        rttinit = 1;
    }

    ++client_send_header.seq;

    struct iovec iovsend[2];
    struct iovec iovrecv[2];

    client_send_msg.msg_name = &dest->address;
    client_send_msg.msg_namelen = dest->size;
    client_send_msg.msg_iov = iovsend;
    client_send_msg.msg_iovlen = 2;
    iovsend[0].iov_base = &client_send_header;
    iovsend[0].iov_len = sizeof(struct rudp_header);
    iovsend[1].iov_base = send_pkt;
    iovsend[1].iov_len = send_size;

    client_recv_msg.msg_name = NULL;
    client_recv_msg.msg_namelen = 0;
    client_recv_msg.msg_iov = iovrecv;
    client_recv_msg.msg_iovlen = 2;
    iovrecv[0].iov_base = &client_recv_header;
    iovrecv[0].iov_len = sizeof(struct rudp_header);
    iovrecv[1].iov_base = recv_pkt;
    iovrecv[1].iov_len = recv_size;

    signal(SIGALRM, sig_alrm);
    rtt_newpack(&rtt_info);

sendagain:
    client_send_header.ts = rtt_ts(&rtt_info);
    sendmsg(socket->sock, &client_send_msg, 0);

    alarm(rtt_start(&rtt_info));

    if (sigsetjmp(jmpbuf, 1) != 0)
    {
        if (rtt_timeout(&rtt_info) < 0)
        {
            fprintf(stderr, "no response; giving up\n");
            rttinit = 0;
            errno = ETIMEDOUT;

            return;
        }

        goto sendagain;
    }

    ssize_t n;
    do
    {
        n = recvmsg(socket->sock, &client_recv_msg, 0);
    } while (n < sizeof(struct rudp_header) ||
             client_recv_header.seq != client_send_header.seq);

    alarm(0);
    rtt_stop(&rtt_info, rtt_ts(&rtt_info) - client_recv_header.ts);
}
