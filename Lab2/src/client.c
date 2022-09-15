#include "udp.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "util.h"
#include "rudp.h"

int main(void)
{
    // Creamos una conexión UDP
    struct udp_socket_t client;
    udp_client_init(&client, "127.0.0.1", 22223);
    struct listing_pkt List;
    struct request_pkt Answer;
    memset(&Answer, 0, sizeof(Answer));

    printf("Quiere ver el Listado de archivos a descargar:\n[1] Obtener Listado\n[2] Salir\n");
    scanf("%d", &Answer.type);
    if (Answer.type != 1 && Answer.type != 2)
    {
        do
        {
            printf("Porfavor escoja una alternativa correcta:\n[1] Obtener Listado\n[2] Salir\n");
            scanf("%d", &Answer.type);
        } while (Answer.type != 1 && Answer.type != 2);
    }
    if (Answer.type == 2)
    {
        exit(1);
    }
    int nro;
    /**** 1 . Envia que quiere obtener el Listado de Archivos *****/
    rudp_client_send_recv(&client, &client.address, (struct request_pkt *)&Answer, sizeof(Answer), &List, sizeof(List));
    printf("         Listado de archivos         \n");
    for (int i = 0; i < List.files_no; i++)
    {
        printf("[%d] %s ---> Tamaño : %zu\n", i, List.files[i].name, List.files[i].size);
    }
    printf("Elija el archivo a obtener, coloque el numero correspondiente al archivo: ");
    scanf("%d", &nro);
    Answer.Filenum = nro;
    Answer.block_no = 0;
    size_t total = List.files[nro].size / 512;
    size_t resto = List.files[nro].size % 512;
    if (resto != 0)
        total++;
    struct data_pkt Data;
    size_t Falta = List.files[nro].size;
    FILE *f_out;
    if ((f_out = fopen(List.files[nro].name, "wb")) == NULL)
    {
        fprintf(stderr, "%s: could not open output file\n", List.files[nro].name);
        return 1;
    }
    size_t LOAD = 0;
    size_t DOWNLOAD = 0;
    while (total > Answer.block_no)
    {
        struct data_pkt Data;
        memset(&Data.data, '\0', sizeof(Data.data));
        Answer.type = 0;
        rudp_client_send_recv(&client, &client.address, (struct request_pkt *)&Answer, sizeof(Answer), &Data, sizeof(Data));
        write_file(f_out, Data.data, Data.block_size);
        Answer.block_no++;
        LOAD += Data.block_size;
        DOWNLOAD = (LOAD * 100) / List.files[nro].size;
        printf("Descarga: %zu\n", DOWNLOAD);
        sleep(1);
    }
    return 0;
}
