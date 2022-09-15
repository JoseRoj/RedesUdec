#include <stdio.h>

#include "rudp.h"
#include "udp.h"
#include "dirent.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(int argv, char *arg[])
{
    /***** Levantar el servidor UDP ******/
    struct udp_socket_t server;
    udp_server_init(&server, 22223);

    /************************** Crear lista de archivos ***********************/
    DIR *d;
    struct dirent *dir;
    struct listing_pkt List; // = malloc(sizeof(struct listing_pkt));
    d = opendir(arg[1]);
    memset(&List, 0, sizeof(List));
    if (d)
    {
        dir = readdir(d); // .
        dir = readdir(d); // ..
        int count = 0;
        while ((dir = readdir(d)) != NULL)
        {
            if ((strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || (*dir->d_name) == '.') || strcmp(dir->d_name, ".DS_Store") == 0)
                continue;
            char Directory[200];
            strcpy(Directory, arg[1]);
            printf("[%d] %s\n", count, dir->d_name);
            strcat(Directory, dir->d_name);
            // printf("%s\n", Directory);
            FILE *Arch = fopen(Directory, "rb");
            if (Arch == NULL)
            {
                fprintf(stderr, "could not open input file\n");
                return 1;
            }
            strcpy(List.files[count].name, dir->d_name);
            List.files[count].size = file_size(Arch);
            List.files_no++;
            // printf("Tamano : %zu\n", List.files[count].size);
            count++;
            fclose(Arch);
        }
        closedir(d);
    }

    while (1)
    {
        // Aquí guardamos la IP del cliente
        struct addr_t client_addr;

        struct request_pkt Client;
        /************ 1 **************/
        rudp_server_recv(&server, &client_addr, &Client, sizeof(Client));
        if (Client.type == 1)
        {
            /*********** 1. 1 Enviar Listado *******************/
            rudp_server_send(&server, &client_addr, (struct listing_pkt *)&List, sizeof(List));
            continue;
        }
        /****** 2 Recibir el archivo que quiere el cliente *******/

        // printf("%d\n", client_addr.address.sin_port);

        char Filesend[200];
        strcpy(Filesend, arg[1]);
        strcat(Filesend, List.files[Client.Filenum].name);
        FILE *Archi = fopen(Filesend, "rb");
        if (Archi == NULL)
        {
            printf("%s\n", Filesend);
            fprintf(stderr, "could not open input file 2\n");
            return 1;
        }
        /************** 2. Enviar archivo al cliente *******/
        size_t sizeBlock = 512;
        size_t ByteArch = 512 * Client.block_no;
        size_t Resta = List.files[Client.Filenum].size - ByteArch;
        if (Resta < 512)
        {
            sizeBlock = List.files[Client.Filenum].size - ByteArch;
        }
        struct data_pkt Data;
        memset(&Data, 0, sizeof(Data));
        Data.block_no = Client.block_no;
        Data.block_size = sizeBlock;

        fseek(Archi, 512 * Client.block_no, SEEK_SET);
        fread(Data.data, Data.block_size, 1, Archi);
        fclose(Archi);
        rudp_server_send(&server, &client_addr, (struct data_pkt *)&Data, sizeof(Data));
    }

    return 0;
}
