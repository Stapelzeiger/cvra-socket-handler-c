#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>

#define HANDLER_SUCCESS       0
#define HANDLER_MALLOC_FAILED 1


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
    // array of open sockets with buffers
    rcv_connection_t *connections;
    int connections_array_sz;
    int number_of_connections;
    // array of callback functions for registered types
    type_callback_t *types;
    int types_array_sz;
    int number_of_types;
    // callback fn for unknown types
    void (*unknown_type_callback)(uint8_t *hash, int socket);
} rcv_handler_t;


void rcv_handler_init(rcv_handler_t *handler);
int rcv_handler_receive(rcv_handler_t *handler);
int rcv_handler_add_socket(rcv_handler_t *handler, int socket);
int rcv_handler_remove_socket(rcv_handler_t *handler, int socket);
int rcv_handler_register_type(rcv_handler_t *handler, 
    serialization_type_t *type, void (*callback)(void *type, int socket));
int rcv_handler_forget_type(serialization_type_t *type);



void rcv_handler_init(rcv_handler_t *handler)
{
    handler->connections = NULL;
    handler->number_of_connections = 0;
    handler->types = NULL;
    handler->number_of_types = 0;
    handler->unknown_type_callback = NULL;
}


#define LIST_SUCCESS      0
#define LIST_ALLOC_FAILED 1

// cmp_fn return values
#define LIST_COMP_SMALLER_THAN -1
#define LIST_COMP_EQUAL         0
#define LIST_COMP_GREATER_THAN  1

typedef struct {
    void *array;
    size_t elem_sz;
    int arraylen;
    int nb_entries;
    int (*cmp_fn)(void *key, void *elem);
} sorted_list_t;

int sorted_list_add(sorted_list_t *list, void *elem, void *key)
{
    if (list->nb_entries + 1 > list->arraylen) {
        // allocate more memory
        int new_len = list->len * 3/2 + 1;
        void *mem = realloc(list->array, list->elem_sz * new_len);
        if (mem == NULL)
            return LIST_ALLOC_FAILED;
        list->array = mem;
        list->arraylen = new_len;
    }
    int i = 0;
    // while new element > entry_i
    while (list->cmp_fn(key, list->array + list->elem_sz*i) == LIST_COMP_GREATER_THAN) {
        i++;
        if (i = list->nb_entries)
            break;
    }
    // insert before i
    memmove(list->array + list->elem_sz * (i + 1),
        list->array + list->elem_sz * i,
        list->elem_sz * (list->nb_entries - i))


    list->nb_entries += 1;
    return LIST_SUCCESS;
}

void *sorted_list_find(sorted_list_t *list, void (*))

int rcv_handler_add_socket(rcv_handler_t *handler, int socket)
{
    if (handler->connections == NULL)
        handler->connections = malloc(sizeof(rcv_connection_t)*)
    return HANDLER_SUCCESS;
}


int main(void)
{
    printf("hello world\n");
    return 0;
}
