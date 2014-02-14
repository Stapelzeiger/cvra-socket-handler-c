#include <string.h>
#include <stdlib.h>

#include "simple_map.h"

#define ELEM_I(map, i) ((map)->array + (map)->elem_sz*(i))

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
        int new_len = map->arraylen * 3/2 + 1;
        void *mem = realloc(map->array, map->elem_sz * new_len);
        if (mem == NULL)
            return SIMPLE_MAP_ALLOC_FAILED;
        map->array = mem;
        map->arraylen = new_len;
    }
    int i = 0;
    if (map->nb_entries > 0) {
        // while new element > entry_i
        while (map->cmp_fn(key, ELEM_I(map, i)) == SIMPLE_MAP_COMP_GREATER_THAN) {
            i++;
            if (i == map->nb_entries) // i points to the first unused field
                break;
        }
        if (map->cmp_fn(key, ELEM_I(map, i)) == SIMPLE_MAP_COMP_EQUAL)
            return SIMPLE_MAP_KEY_EXISTS;
    }
    // insert before entry_i
    memmove(ELEM_I(map, (i + 1)), ELEM_I(map, i),
        map->elem_sz * (map->nb_entries - i));
    memcpy(ELEM_I(map, i), elem, map->elem_sz);

    map->nb_entries += 1;
    return SIMPLE_MAP_SUCCESS;
}

int simple_map_remove(simple_map_t *map, void *key)
{
    void *elem = simple_map_find(map, key);
    if (elem == NULL)
        return SIMPLE_MAP_KEY_NOT_FOUND;

    // TODO remove elem

    // TODO shrink memory

    return SIMPLE_MAP_SUCCESS;
}

// returns a pointer to the element or NULL if the element isn't in the map
void *simple_map_find(simple_map_t *map, void *key)
{
    // binary search in sorted array
    int a, b, i;
    a = 0, b = map->nb_entries - 1;
    while (b >= a) {
        i = (a + b)/2;
        int cmp = map->cmp_fn(key, ELEM_I(map, i));
        if (cmp == SIMPLE_MAP_COMP_EQUAL)
            return ELEM_I(map, i);
        if (cmp == SIMPLE_MAP_COMP_GREATER_THAN) {
            a = i + 1;
        } else {
            b = i - 1;
        }
    }
    return NULL;
}

