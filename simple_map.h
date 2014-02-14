// simple_map
// a (very basic) map data structure
// the key is inside the element-struct stored in the map
// the user has to supply a compare function for comparing keys
// adding / removing is slow (O(n)), finding is faster (O(log(n))) using binary
// search. This data structure is intended for very small maps (~20-30 entries)

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
    int (*cmp_fn)(void *key, void *elem));
int simple_map_add(simple_map_t *map, void *elem, void *key);
int simple_map_remove(simple_map_t *map, void *key);
void *simple_map_find(simple_map_t *map, void *key);
