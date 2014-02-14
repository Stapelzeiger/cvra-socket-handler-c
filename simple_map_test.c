
#include <stdio.h>

#include "simple_map.h"

struct test_data {
    float x;
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

    struct test_data a = {3.14, 42};

    simple_map_add(&map, &a, &a.key);

    int key = 42;
    struct test_data *res = simple_map_find(&map, &key);
    printf("%f\n", res->x);

    printf("All tests passed\n");
    return 0;
}