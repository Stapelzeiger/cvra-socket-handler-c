
#include <stdio.h>
#include <assert.h>

#include "simple_map.h"

struct test_data {
    int x;
    int key;
};

int test_data_cmp(void *key, void *elem)
{
    if (*(int *)key == ((struct test_data *)elem)->key)
        return SIMPLE_MAP_COMP_EQUAL;
    if (*(int *)key > ((struct test_data *)elem)->key)
        return SIMPLE_MAP_COMP_GREATER_THAN;
    return SIMPLE_MAP_COMP_SMALLER_THAN;
}


int main(void)
{
    simple_map_t map;
    simple_map_init(&map, sizeof(struct test_data), test_data_cmp);

    struct test_data a = {314, 42};
    int key;
    struct test_data *res;
    simple_map_add(&map, &a, &a.key);
    key = 42;
    res = simple_map_find(&map, &key);
    assert(res->x == 314);

    key = 24567; // nonexistant key
    assert(simple_map_find(&map, &key) == NULL);

    assert(simple_map_add(&map, &a, &a.key) == SIMPLE_MAP_KEY_EXISTS);

    a.key = 23;
    a.x = 3445;
    simple_map_add(&map, &a, &a.key);
    key = 42;
    res = simple_map_find(&map, &key);
    assert(res->x == 314);
    key = 23;
    res = simple_map_find(&map, &key);
    assert(res->x == 3445);

    key = 24567; // nonexistant key
    assert(simple_map_find(&map, &key) == NULL);

    a.key = 50;
    a.x = 1337;
    simple_map_add(&map, &a, &a.key);
    key = 42;
    res = simple_map_find(&map, &key);
    assert(res->x == 314);
    key = 23;
    res = simple_map_find(&map, &key);
    assert(res->x == 3445);
    key = 50;
    res = simple_map_find(&map, &key);
    assert(res->x == 1337);

    key = 24567; // nonexistant key
    assert(simple_map_find(&map, &key) == NULL);

    printf("All tests passed\n");
    return 0;
}