#include <string.h>
#include "data.h"

const serialization_fn_table_t data_fn_table = {
    .hash = {130, 123, 61, 48, 111, 86, 30, 52},
    .serialize = data_serialize,
    .serialized_size = data_serialized_size,
    .deserialize = data_deserialize,
    .alloc_size = data_alloc_size};



static void *alloc(int size, void **alloc_mem)
{
    void *mem = *alloc_mem;
    alloc_mem += (size-1 | (8-1)) + 1;
    return mem;
}

static uint8_t *string_serialize(char *str, uint8_t *buffer)
{
    int len = strlen(str);
    *buffer++ = (int8_t)(((int16_t)len)>>8);
    *buffer++ = (int8_t)(((int16_t)len)>>0);
    int i; // todo: use strncpy()
    for (i = 0; i < len; i++) {
        *buffer++ = *str++;
    }
    return buffer;
}

static uint8_t *string_deserialize(char **str, uint8_t *buffer, void **alloc_mem)
{
    int16_t len = (((int16_t)*buffer++)<<8);
    len += ((int16_t)*buffer++);
    *str = alloc((len+1)*sizeof(char), alloc_mem);
    strncpy(*str, (char*)buffer, len);
    (*str)[len] = '\0';
    buffer += len*sizeof(char);
    return buffer;
}


// data
uint8_t *data_serialize(data_t  *data, uint8_t *buffer)
{
    *buffer++ = (int8_t)((*(int32_t*)&data->a)>>24);
    *buffer++ = (int8_t)((*(int32_t*)&data->a)>>16);
    *buffer++ = (int8_t)((*(int32_t*)&data->a)>>8);
    *buffer++ = (int8_t)((*(int32_t*)&data->a)>>0);
    return buffer;
}

size_t data_serialized_size(data_t  *data)
{
    return 4;
}

data_t *data_deserialize(uint8_t *buffer, void *alloc_mem)
{
    data_t  *data = alloc(sizeof(data_t ), &alloc_mem);
    _data_deserialize_into(data, buffer, &alloc_mem);
    return data;
}

uint8_t *_data_deserialize_into(data_t  *data, uint8_t *buffer, void **alloc_mem)
{
    *(int32_t*)&data->a = 0;
    *(int32_t*)&data->a += (((int32_t)*buffer++)<<24);
    *(int32_t*)&data->a += (((int32_t)*buffer++)<<16);
    *(int32_t*)&data->a += (((int32_t)*buffer++)<<8);
    *(int32_t*)&data->a += (((int32_t)*buffer++)<<0);
    return buffer;
}

size_t _data_dynamic_alloc_size(uint8_t **buffer, uint8_t *buffend)
{
    return 4; // 16k
}

size_t data_alloc_size(uint8_t *buffer, size_t len)
{
    return 4; // 16k
}

