#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <poll.h>

#include <stdio.h>

#include "connection.h"


typedef struct rcv_connection_s {
    int socket;
    uint8_t header[HEADER_LENGTH];
    int header_len;
    int length;
    type_callback_t *type;
    uint8_t *buffer;
    int buffer_len;
} rcv_connection_t;

struct send_buffer_s {
    int socket;
    uint8_t *buffer;
    int buffer_len;
    int buffer_pos;
    send_buffer_t *next;    // for singly-linked list
};

typedef struct send_descriptor_count_s {
    int socket;
    int count;
} send_descriptor_count_t;


// local function descriptors
int _receive_package(rcv_handler_t *h, int socket);
int32_t _get_header_length(uint8_t header[HEADER_LENGTH]);


static rcv_connection_t *rcv_connection_for_socket(rcv_handler_t *h, int socket)
{
    return simple_map_find(&h->connections, (void*)&socket);
}

static type_callback_t *type_callback_for_hash(rcv_handler_t *h, uint8_t *hash)
{
    return simple_map_find(&h->types, (void*)hash);
}

static int descriptor_count_for_socket(send_handler_t *h, int socket)
{
    return *((int*)simple_map_find(&h->descriptor_count, (void*)&socket));
}

static int _map_connection_cmp_fn(void *key, void *conn)
{
    if (*(int *)key == ((rcv_connection_t*)conn)->socket)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((rcv_connection_t*)conn)->socket)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    return SIMPLE_MAP_COMP_SMALLER_THAN;
}

static int _map_pollfd_cmp_fn(void *key, void *fd)
{
    if (*(int *)key == ((struct pollfd*)fd)->fd)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((struct pollfd*)fd)->fd)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    return SIMPLE_MAP_COMP_SMALLER_THAN;
}

static int _map_type_cb_cmp_fn(void *key, void *type)
{
    int cmp = strncmp((char*)key, (char*)((type_callback_t*)type)->type->hash, 8);
    if (cmp == 0)
        return SIMPLE_MAP_COMP_EQUAL;
    else if (cmp > 0)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    else
        return SIMPLE_MAP_COMP_SMALLER_THAN;
}

static int _map_desc_count_cmp_fn(void *key, void *desc_count)
{
    if (*(int *)key == ((send_descriptor_count_t*)desc_count)->socket)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((send_descriptor_count_t*)desc_count)->socket)
        return SIMPLE_MAP_COMP_GREATER_THAN;
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
    conn.header_len = 0;
    conn.length = 0;
    conn.type = NULL;
    conn.buffer = NULL;
    conn.buffer_len = 0;

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
    ret = poll(fds, handler->polls.nb_entries, POLL_TIMEOUT_MSECS);

    // handle
    if (ret > 0) {
        for (i = 0; i < handler->polls.nb_entries; i++) {
            // TODO maybe check first if (fds[i].revents)
            if (fds[i].revents & POLLIN) {
                // receive ready
#ifdef DEBUG
                printf("Receive ready.\n");
#endif
                _receive_package(handler, fds[i].fd);
                return HANDLER_SUCCESS;
            }
            if (fds[i].revents & POLLPRI) {
#ifdef DEBUG
                printf("High priority\n");
#endif
                // TODO high-priority receive ready
            }
            if (fds[i].revents & POLLHUP) {
                // hang up
#ifdef DEBUG
                printf("Hang up\n");
#endif
                if (shutdown(fds[i].fd, SHUT_RDWR) == 0) {
                    // TODO shutdown successful
                }
                rcv_handler_remove_socket(handler, fds[i].fd);
            }
            if (fds[i].revents & POLLERR) {
                // error
#ifdef DEBUG
                printf("Error\n");
#endif
                if (shutdown(fds[i].fd, SHUT_RDWR) == 0) {
                    // TODO shutdown successful
                }
                rcv_handler_remove_socket(handler, fds[i].fd);
            }
            if (fds[i].revents & POLLNVAL) {
                // invalid file descriptor
#ifdef DEBUG
                printf("Inval\n");
#endif
                rcv_handler_remove_socket(handler, fds[i].fd);
            }
        }
    }
    else if (ret == 0) {
        // TODO timeout
        return HANDLER_SUCCESS;
    }
    else if (ret < 0) {
        // TODO errno
    }

    return HANDLER_UNKNOWN_ERROR;
}

int _receive_package(rcv_handler_t *h, int socket)
{
    int ret;
    rcv_connection_t *conn = NULL;
    conn = rcv_connection_for_socket(h, socket);
    if (conn != NULL) {
        if (conn->header_len < HEADER_LENGTH) {
            ret = recv(socket, conn->header + conn->header_len,
                    HEADER_LENGTH - conn->header_len, 0);
            if (ret > 0)
                conn->header_len += ret;
            else if (ret == 0) {
                // poll says input ready but nothing received: peer closed sock
#ifdef DEBUG
                printf("Peer closed socket %d.\n", socket);
#endif
                if (shutdown(socket, SHUT_RDWR) == 0) {
                    // TODO shutdown successful
                }
                rcv_handler_remove_socket(h, socket);
            }
            else {
                // TODO error receiving
            }
            return 0;    // need to poll again to be non-blocking
        }
        if (conn->header_len == HEADER_LENGTH) {
#ifdef DEBUG
            printf("Header received.\n");
#endif
            if (conn->buffer == NULL) {
                conn->length = _get_header_length(conn->header) - HEADER_LENGTH;
#ifdef DEBUG
                printf("Length: %d\n", conn->length);
                printf("Hash: %s\n", &conn->header[HEADER_LENGTH_FIELD]);
#endif
                conn->type = type_callback_for_hash(h,
                        &conn->header[HEADER_LENGTH_FIELD]);
                if (conn->type == NULL) {
                    // TODO unkown type
                    printf("Unknown type.\n");
                    if (conn->length > ACCEPTED_GARBAGE) {
                        // too much, close socket
                        if (shutdown(socket, SHUT_RDWR) == 0) {
                            // TODO shutdown successful
                        }
                        else {
#ifdef DEBUG
                            printf("Couldn't shutdown.\n");
#endif
                        }
                        if (rcv_handler_remove_socket(h, socket) != HANDLER_SUCCESS)
#ifdef DEBUG
                            printf("Couldn't remove.\n");
#endif
                        return 0;   // TODO
                    }
                    else {
                        // receive and discard
                    }
                }
                conn->buffer = (uint8_t*)malloc(conn->length*sizeof(uint8_t));
                if (conn->buffer == NULL)
                    // TODO malloc fail or length == 0 (should be handled)
                    ;
            }
            ret = recv(socket, conn->buffer + conn->buffer_len,
                    conn->length - conn->buffer_len, 0);
            if (ret >= 0)
                conn->buffer_len += ret;
            else if (ret == 0) {
                // poll says input ready but nothing received: peer closed sock
#ifdef DEBUG
                printf("Peer closed socket %d.\n", socket);
#endif
                if (shutdown(socket, SHUT_RDWR) == 0) {
                    // TODO shutdown successful
                }
                rcv_handler_remove_socket(h, socket);
            }
            else {
                // TODO error receiving
            }

            if (conn->length == conn->buffer_len) {
                // the whole message has been received
                if (conn->type != NULL)
                    conn->type->callback(
                        conn->type->type->deserialize(conn->buffer, conn->length)
                        , socket);
                // clear all the buffers
                conn->header_len = 0;
                conn->length = 0;
                conn->type = NULL;
                free(conn->buffer);
                conn->buffer = NULL;
                conn->buffer_len = 0;
            }
        }
    }
    else {
#ifdef DEBUG
        printf("conn == NULL\n");
#endif
    }
    return 0;   // TODO
}

int32_t _get_header_length(uint8_t header[HEADER_LENGTH])
{
    int32_t var = 0;
    var += (((int32_t)*header++)<<24);
    var += (((int32_t)*header++)<<16);
    var += (((int32_t)*header++)<<8);
    var += (((int32_t)*header++)<<0);
#ifdef DEBUG
    printf("get length: %d\n", var);
#endif
    return var;
}



// Send handler functions
void send_handler_init(send_handler_t *handler)
{
    handler->buffers = NULL;
    simple_map_init(&handler->polls, sizeof(struct pollfd), _map_pollfd_cmp_fn);
    simple_map_init(&handler->descriptor_count, sizeof(send_descriptor_count_t),
            _map_desc_count_cmp_fn);
}

int send_handler_send_package(  send_handler_t *h,
                                int socket,
                                void *message,
                                serialization_type_t *type)
{
    send_buffer_t *prev_buffer = h->buffers;
    send_buffer_t *new_buffer = NULL;

    if (prev_buffer != NULL) {
        // find end of linked list
        while (prev_buffer->next != NULL) {
            prev_buffer = prev_buffer->next;
        }
        new_buffer = malloc(sizeof(send_buffer_t));
        if (new_buffer == NULL) {
            return HANDLER_MALLOC_FAILED;
        }
        new_buffer->socket = socket;
        // TODO this will change
        type->serialize(message, new_buffer->buffer, &new_buffer->buffer_len);
        new_buffer->buffer_pos = 0;
        new_buffer->next = NULL;
        prev_buffer->next = new_buffer;

        // count messages per socket
        send_descriptor_count_t *count;
        count = simple_map_find(&h->descriptor_count, (void*)&socket);
        if (count == NULL) {
            // count is zero
            send_descriptor_count_t new_count = { socket, 1 };
            if (simple_map_add(&h->descriptor_count, &new_count,
                        &new_count.socket) != SIMPLE_MAP_SUCCESS) {
                return HANDLER_UNKNOWN_ERROR;
            }

            // register socket for polling
            struct pollfd fd;
            fd.fd = socket;
            fd.events = POLLOUT;

            if (simple_map_add(&h->polls, &fd, &fd.fd) == SIMPLE_MAP_ALLOC_FAILED)
                return HANDLER_MALLOC_FAILED;
        }
        else {
            count->count++;
        }
    }
    else {
        // first entry in the list of buffers
        // this is a bit copypasta, but it saves a few CPU cycles
        prev_buffer = malloc(sizeof(send_buffer_t));
        if (prev_buffer == NULL) {
            return HANDLER_MALLOC_FAILED;
        }
        prev_buffer->socket = socket;
        // TODO this will change
        type->serialize(message, prev_buffer->buffer, &prev_buffer->buffer_len);
        prev_buffer->buffer_pos = 0;
        prev_buffer->next = NULL;

        // count messages per socket
        // count is zero
        send_descriptor_count_t new_count = { socket, 1 };
        if (simple_map_add(&h->descriptor_count, &new_count,
                    &new_count.socket) != SIMPLE_MAP_SUCCESS) {
            return HANDLER_UNKNOWN_ERROR;
        }

        // register socket for polling
        struct pollfd fd;
        fd.fd = socket;
        fd.events = POLLOUT;

        if (simple_map_add(&h->polls, &fd, &fd.fd) == SIMPLE_MAP_ALLOC_FAILED)
            return HANDLER_MALLOC_FAILED;
    }
}
