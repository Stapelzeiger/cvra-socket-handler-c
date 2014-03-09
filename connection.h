#include <stdint.h>
#include "simple_map.h"

#define HANDLER_SUCCESS       0
#define HANDLER_MALLOC_FAILED 1
#define HANDLER_UNKNOWN_ERROR 2

#define POLL_TIMEOUT_MSECS 5
#define ACCEPTED_GARBAGE 1024

#define HEADER_LENGTH_FIELD 4
#define HEADER_UID_FIELD    8
#define HEADER_LENGTH (HEADER_UID_FIELD + HEADER_LENGTH_FIELD)



// TODO from serialization file
typedef struct serialization_type_s {
    const uint8_t hash[8];
    const void (*serialize)(void *message, uint8_t **buffer, int *length);
    const void *(*deserialize)(uint8_t *buffer, int length);
} serialization_type_t;

typedef struct receive_handler_s {
    // map socket -> rcv_connection_t
    simple_map_t connections;
    // map a pollfd structure onto itslef (key is the file descriptor)
    // welp, this is a hack...
    simple_map_t polls;
    // map hash -> type_callback_t
    simple_map_t types;
    // callback fn for unknown types
    void (*unknown_type_callback)(uint8_t *hash, int socket);
} rcv_handler_t;


typedef struct send_buffer_s send_buffer_t;

typedef struct send_handler_s {
    // FIFO buffer of buffers/packages (singly linked list)
    send_buffer_t *buffers;
    // map a pollfd structure onto itslef (key is the file descriptor)
    simple_map_t polls;
    // map socket -> number of messages left to send
    simple_map_t descriptor_count;
}send_handler_t;

typedef struct type_callback_s {
    serialization_type_t *type;
    void (*callback)(const void *message, int socket);
}type_callback_t;


void rcv_handler_init(rcv_handler_t *handler);
int rcv_handler_handle(rcv_handler_t *handler);
int rcv_handler_add_socket(rcv_handler_t *handler, int socket);
int rcv_handler_remove_socket(rcv_handler_t *handler, int socket);
int rcv_handler_register_type(rcv_handler_t *handler,
    serialization_type_t *type, void (*callback)(const void *message, int socket));
int rcv_handler_forget_type(rcv_handler_t *handler, serialization_type_t *type);

void send_handler_init(send_handler_t *handler);
int send_handler_handle(send_handler_t *h);
int send_handler_send_package(  send_handler_t *h,
                                int socket,
                                void *message,
                                serialization_type_t *type);
