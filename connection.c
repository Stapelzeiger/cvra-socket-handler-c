#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <poll.h>

#include "simple_map.h"

#define HANDLER_SUCCESS       0
#define HANDLER_MALLOC_FAILED 1
#define HANDLER_UNKNOWN_ERROR 2

#define TIMEOUT_MSECS 5


// todo from serialization file
typedef struct serialization_type_s {
    const uint8_t hash[8];
    void (*const serialize)(void); // todo
    void (*const deserialize)(void);
} serialization_type_t;


typedef struct rcv_connection_s {
    int socket;
    // buffers
} rcv_connection_t;

typedef struct type_callback_s {
    serialization_type_t *type;
    void (*callback)(void *type, int socket);
} type_callback_t;

typedef struct receive_handler_s {
    // map socket -> rcv_connection_t
    simple_map_t connections;
    // map socket -> pollfd (poll requests)
    simple_map_t polls;
    // map hash -> type_callback_t
    simple_map_t types;
    // callback fn for unknown types
    void (*unknown_type_callback)(uint8_t *hash, int socket);
} rcv_handler_t;


void rcv_handler_init(rcv_handler_t *handler);
int rcv_handler_receive(rcv_handler_t *handler);
int rcv_handler_add_socket(rcv_handler_t *handler, int socket);
int rcv_handler_remove_socket(rcv_handler_t *handler, int socket);
int rcv_handler_register_type(rcv_handler_t *handler,
    serialization_type_t *type, void (*callback)(void *type, int socket));
int rcv_handler_forget_type(rcv_handler_t *handler, serialization_type_t *type);


static rcv_connection_t *rcv_connection_for_socket(rcv_handler_t *h, int socket)
{
    return simple_map_find(&h->connections, (void*)&socket);
}

static type_callback_t *type_callback_for_hash(rcv_handler_t *h, uint8_t *hash)
{
    return simple_map_find(&h->types, (void*)&hash);
}

static int _map_connection_cmp_fn(void *key, void *conn)
{
    if (*(int *)key == ((rcv_connection_t*)conn)->socket)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((rcv_connection_t*)conn)->socket)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    return SIMPLE_MAP_COMP_SMALLER_THAN;
}

static int _map_pollfd_cmp_fn(void *key, void *fd){
    if (*(int *)key == ((struct pollfd*)fd)->fd)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((struct pollfd*)fd)->fd)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    return SIMPLE_MAP_COMP_SMALLER_THAN;
}

static int _map_type_cb_cmp_fn(void *key, void *type)
{
    int cmp = strncmp((char*)key, (char*)&((type_callback_t*)type)->type->hash, 8);
    if (cmp == 0)
        return SIMPLE_MAP_COMP_EQUAL;
    else if (cmp > 0)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    else
        return SIMPLE_MAP_COMP_SMALLER_THAN;
}

void rcv_handler_init(rcv_handler_t *handler)
{
    simple_map_init(&handler->connections, sizeof(rcv_connection_t), _map_connection_cmp_fn);
    simple_map_init(&handler->types, sizeof(type_callback_t), _map_type_cb_cmp_fn);
    simple_map_init(&handler->polls, sizeof(struct pollfd), _map_pollfd_cmp_fn);
    handler->unknown_type_callback = NULL;
}

int rcv_handler_add_socket(rcv_handler_t *handler, int socket)
{
    rcv_connection_t conn;
    struct pollfd fd;
    conn.socket = socket;
    fd.fd = socket;
    fd.events = POLLIN | POLLPRI;
    if (simple_map_add(&handler->connections, &conn, &conn.socket) != SIMPLE_MAP_SUCCESS)
        return HANDLER_UNKNOWN_ERROR;
    if (simple_map_add(&handler->polls, &fd, &fd.fd) != SIMPLE_MAP_SUCCESS)
        return HANDLER_UNKNOWN_ERROR;
    return HANDLER_SUCCESS;
}

int rcv_handler_remove_socket(rcv_handler_t *handler, int socket)
{
    if (simple_map_remove(&handler->connections, &socket) != SIMPLE_MAP_SUCCESS)
        return HANDLER_UNKNOWN_ERROR;
    if (simple_map_remove(&handler->polls, &socket) != SIMPLE_MAP_SUCCESS)
        return HANDLER_UNKNOWN_ERROR;
    return HANDLER_SUCCESS;
}

int rcv_handler_register_type(rcv_handler_t *handler,
    serialization_type_t *type, void (*callback)(void *type, int socket))
{
    type_callback_t tc;
    tc.type = type;
    tc.callback = callback;
    if (simple_map_add(&handler->types, &tc, (void*)&tc.type->hash) == SIMPLE_MAP_SUCCESS)
        return HANDLER_SUCCESS;
    return HANDLER_UNKNOWN_ERROR;
}

int rcv_handler_forget_type(rcv_handler_t *handler, serialization_type_t *type)
{
    if (simple_map_remove(&handler->connections, (void*)&type->hash) != SIMPLE_MAP_SUCCESS)
        return HANDLER_UNKNOWN_ERROR;
    return HANDLER_SUCCESS;
}

int rcv_handler_handle(rcv_handler_t *handler)
{
    int ret, i;
    struct pollfd *fds = (struct pollfd*)handler->polls.array;
    ret = poll(fds, handler->polls.nb_entries, TIMEOUT_MSECS);

    // handle
    if (ret > 0) {
        for (i = 0; i < handler->polls.nb_entries; i++) {
            if (fds[i].revents & POLLIN) {
                // receive ready
            }
            if (fds[i].revents & POLLPRI) {
                // high-priority receive ready
            }
            if (fds[i].revents & POLLHUP) {
                // hang up
            }
            if (fds[i].revents & POLLERR) {
                // error
            }
            if (fds[i].revents & POLLNVAL) {
                // invalid file descriptor
            }
        }
    }
    else if (ret == 0) {
        // timeout
    }
    else if (ret < 0) {
        // TODO errno
    }

    return HANDLER_UNKNOWN_ERROR;
}



int main(void)
{
    printf("hello world\n");
    return 0;
}
