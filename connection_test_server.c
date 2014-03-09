#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "connection.h"


const void serialize(void *message, uint8_t **buffer, int *length);

int main(void)
{
    send_handler_t handler;

    send_handler_init(&handler);



    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    int yes = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("listening\n");

    listen(listenfd, 1);

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    printf("connected\n");

    data_t data = {.a = 42};

    int i;
    for (i = 0; i < 10000; i++) {
        send_handler_send_package(&handler, connfd, &data, &data_fn_table);
        data.a++;
    }

    printf("sent\n");

    while(1) {
        //printf("handle\n");
        send_handler_handle(&handler);
        //sleep(1);
    }
}

