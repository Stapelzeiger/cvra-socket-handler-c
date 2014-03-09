#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "connection.h"

typedef struct data_s {
    int32_t a;
} data_t;

const void serialize(void *message, uint8_t **buffer, int *length);

int main(void)
{
    send_handler_t handler;
    serialization_fn_table_t serial = {.hash = "asdfasdf",
                                    .serialize = serialize,
                                    .deserialize = NULL};

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

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    printf("listening\n");

    listen(listenfd, 1);

    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

    printf("connected\n");

    data_t data = {.a = 42};

    int i;
    for (i = 0; i < 10000; i++) {
        send_handler_send_package(&handler, connfd, &data, &serial);
        data.a++;
    }

    printf("sent\n");

    while(1) {
        //printf("handle\n");
        send_handler_handle(&handler);
        //sleep(1);
    }
}

const void serialize(void *message, uint8_t **buffer, int *length)
{
    *length = 16;
    uint8_t *b = malloc(*length*sizeof(uint8_t));
    if (b == NULL) printf("buffer malloc failed\n");
    b[0] = (uint8_t)(((int32_t)*length)>>24);
    b[1] = (uint8_t)(((int32_t)*length)>>16);
    b[2] = (uint8_t)(((int32_t)*length)>> 8);
    b[3] = (uint8_t)(((int32_t)*length)>> 0);
    b[4] = 'a';
    b[5] = 's';
    b[6] = 'd';
    b[7] = 'f';
    b[8] = 'a';
    b[9] = 's';
    b[10] = 'd';
    b[11] = 'f';
    b[12] = (uint8_t)((((data_t*)message)->a)>>24);
    b[13] = (uint8_t)((((data_t*)message)->a)>>16);
    b[14] = (uint8_t)((((data_t*)message)->a)>> 8);
    b[15] = (uint8_t)((((data_t*)message)->a)>> 0);
    *buffer = b;
}
