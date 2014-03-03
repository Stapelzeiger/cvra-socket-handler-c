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
struct serialization_type_s {
    const uint8_t hash[8];
    void const (*serialize)(void); // todo
    void const *(*deserialize)(uint8_t *buffer, int length);
};

typedef struct serialization_type_s serialization_type_t;

struct receive_handler_s {
    // map socket -> rcv_connection_t
    simple_map_t connections;
    // map a pollfd structure onto itslef (key is the file descriptor)
    // welp, this is a hack...
    simple_map_t polls;
    // map hash -> type_callback_t
    simple_map_t types;
    // callback fn for unknown types
    void (*unknown_type_callback)(uint8_t *hash, int socket);
};

typedef struct receive_handler_s rcv_handler_t;

struct type_callback_s {
    serialization_type_t *type;
    void (*callback)(void *message, int socket);
};

typedef struct type_callback_s type_callback_t;

void rcv_handler_init(rcv_handler_t *handler);
int rcv_handler_handle(rcv_handler_t *handler);
int rcv_handler_add_socket(rcv_handler_t *handler, int socket);
int rcv_handler_remove_socket(rcv_handler_t *handler, int socket);
int rcv_handler_register_type(rcv_handler_t *handler,
    serialization_type_t *type, void (*callback)(void *type, int socket));
int rcv_handler_forget_type(rcv_handler_t *handler, serialization_type_t *type);
