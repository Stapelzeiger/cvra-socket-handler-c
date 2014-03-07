#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "connection.h"

typedef struct data_s {
    int32_t a;
} data_t;

data_t *deserialize(uint8_t *buffer, int length);
void callback(const void *message, int socket);

int main(void)
{
    rcv_handler_t handler;
    serialization_type_t serial = {.hash = "asdfasdf",
                                    .serialize = NULL,
                                    .deserialize = (const void*)deserialize};
    type_callback_t type;

    rcv_handler_init(&handler);

    if (rcv_handler_register_type(&handler, &serial, callback) != HANDLER_SUCCESS) {
        printf("Type register failed.\n");
        return EXIT_FAILURE;
    }

    int sockfd = 0;
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection 1 Failed.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Connection 1: %d\n", sockfd);
    }

    if (rcv_handler_add_socket(&handler, sockfd) != HANDLER_SUCCESS) {
        printf("Socket 1 add failed.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serv_addr2;
    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(6000);
    serv_addr2.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2)) < 0) {
        printf("Connection 2 Failed.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Connection 2: %d\n", sockfd);
    }

    if (rcv_handler_add_socket(&handler, sockfd) != HANDLER_SUCCESS) {
        printf("Socket 2 add failed.\n");
        return EXIT_FAILURE;
    }

    while (42) {
        if (rcv_handler_handle(&handler) != HANDLER_SUCCESS) {
           printf("Handle fail.\n");
           return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

data_t *deserialize(uint8_t *buffer, int length)
{
    data_t *data = NULL;

    data = malloc(sizeof(data_t));

    int32_t var = 0;
    var += (((int32_t)*buffer++)<<24);
    var += (((int32_t)*buffer++)<<16);
    var += (((int32_t)*buffer++)<<8);
    var += (((int32_t)*buffer++)<<0);

    data->a = var;

    return data;
}

void callback(const void *message, int socket)
{
    data_t *data = (data_t*)message;
    printf("Callback from socket %d: %d\n", socket, data->a);
}
