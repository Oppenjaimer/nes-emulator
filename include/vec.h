#ifndef NES_VEC_H
#define NES_VEC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VEC_DEFAULT_CAPACITY 8

typedef struct Vec {
    uint8_t *data;      // Allocated memory
    size_t size;        // Current number of elements
    size_t capacity;    // Maximum number of elements
} Vec;

bool vec_init(Vec *vec, size_t capacity);
void vec_free(Vec *vec);
bool vec_push(Vec *vec, uint8_t value);
bool vec_pop(Vec *vec, uint8_t *value);
bool vec_get(const Vec *vec, size_t index, uint8_t *value);
bool vec_set(Vec *vec, size_t index, uint8_t value);

#endif
