
#include "simple_map.h"


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
    // binary search in sorted array
    int a, b, i;
    a = 0, b = map->nb_entries;
    while (a != b) {
        i = (a + b)/2;
        // TODO ...
    }
    return NULL;
}

