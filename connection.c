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




// Error codes (>0 is an error)
#define SIMPLE_MAP_SUCCESS       0
#define SIMPLE_MAP_ALLOC_FAILED  1
#define SIMPLE_MAP_KEY_EXISTS    2
#define SIMPLE_MAP_KEY_NOT_FOUND 3

// cmp_fn return values
#define SIMPLE_MAP_COMP_SMALLER_THAN -1
#define SIMPLE_MAP_COMP_EQUAL         0
#define SIMPLE_MAP_COMP_GREATER_THAN  1
// The compare function is used for sorting / finding an element.
// The compare function takes a pointer to the data structure and a pointer
// to the key, which is a field inside the data structure and returns
// SIMPLE_MAP_COMP_SMALLER_THAN, SIMPLE_MAP_COMP_EQUAL or
// SIMPLE_MAP_COMP_GREATER_THAN.

typedef struct {
    void *array;    // sorted array of datastructs with size elem_sz
    size_t elem_sz; // size of the structure stored in the map
    int arraylen;   // length of the allocated array
    int nb_entries; // number of elements stored
    int (*cmp_fn)(void *key, void *elem); // compare fn pointer, see above
} simple_map_t;


void simple_map_init(simple_map_t *map, size_t elem_sz, 
    int (*cmp_fn)(void *key, void *elem))
{
    map->array = NULL;
    map->elem_sz = elem_sz;
    map->arraylen = 0;
    map->nb_entries = 0;
    map->cmp_fn = cmp_fn;
}

int simple_map_add(simple_map_t *map, void *elem, void *key)
{
    if (map->nb_entries + 1 > map->arraylen) {
        // allocate more memory
        int new_len = map->len * 3/2 + 1;
        void *mem = realloc(map->array, map->elem_sz * new_len);
        if (mem == NULL)
            return SIMPLE_MAP_ALLOC_FAILED;
        map->array = mem;
        map->arraylen = new_len;
    }
    int i = 0;
    // while new element > entry_i
    while (map->cmp_fn(key, map->array + map->elem_sz*i) == SIMPLE_MAP_COMP_GREATER_THAN) {
        i++;
        if (i = map->nb_entries) // i points to the first unused field
            break;
    }
    if (map->cmp_fn(key, map->array + map->elem_sz*i) == SIMPLE_MAP_COMP_EQUAL)
        return SIMPLE_MAP_KEY_EXISTS;
    // insert before entry_i
    memmove(map->array + map->elem_sz * (i + 1),
        map->array + map->elem_sz * i,
        map->elem_sz * (map->nb_entries - i))
    memcpy(map->array + map->elem_sz * i, elem, map->elem_sz);

    map->nb_entries += 1;
    return SIMPLE_MAP_SUCCESS;
}

int simple_map_remove(simple_map_t *map, void *key)
{
    void elem = simple_map_find(map, key);
    if (elem == NULL)
        return SIMPLE_MAP_KEY_NOT_FOUND;

    // TODO remove elem
    
    // TODO shrink memory

    returns SIMPLE_MAP_SUCCESS;
}

// returns a pointer to the element or NULL if the element isn't in the map
void *simple_map_find(simple_map_t *map, void *key)
{
    // TODO binary search
}




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
