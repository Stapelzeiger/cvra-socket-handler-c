#ifndef DATA_H
#define DATA_H

#include <stdint.h>

#ifndef __SERIALIZATION_FN_TABLE
#define __SERIALIZATION_FN_TABLE
typedef struct {
    uint8_t hash[8];
    uint8_t *(*serialize)(void  *data, uint8_t *buffer);
    size_t (*serialized_size)(void  *data);
    void *(*deserialize)(uint8_t *buffer, void *alloc_mem);
    size_t (*alloc_size)(uint8_t *buffer, size_t len);
} serialization_fn_table_t;
#endif

// type definitions

// data:
//
// Test data type.
//
typedef struct {
    int32_t a;
} data_t ;


// function tables

extern const serialization_fn_table_t data_fn_table;

// prototypes

#ifdef __cplusplus
extern "C" {
#endif

// data
uint8_t *data_serialize(data_t  *data, uint8_t *buffer);
size_t data_serialized_size(data_t  *data);
data_t *data_deserialize(uint8_t *buffer, void *alloc_mem);
uint8_t *_data_deserialize_into(data_t  *data, uint8_t *buffer, void **alloc_mem);
size_t _data_dynamic_alloc_size(uint8_t **buffer, uint8_t *buffend);
size_t data_alloc_size(uint8_t *buffer, size_t len);

#ifdef __cplusplus
}
#endif

#endif
