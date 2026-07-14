#include "vec.h"

#include <stdlib.h>

bool vec_init(Vec *vec, size_t capacity) {
    if (!vec) return false;

    vec->size = 0;
    vec->capacity = (capacity > 0) ? capacity : VEC_DEFAULT_CAPACITY;

    vec->data = malloc(vec->capacity * sizeof(uint8_t));
    if (!vec->data) {
        vec->capacity = 0;
        return false;
    }

    return true;
}

void vec_free(Vec *vec) {
    if (!vec || !vec->data) return;

    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

bool vec_push(Vec *vec, uint8_t value) {
    if (!vec || !vec->data) return false;

    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity * 2;
        uint8_t *new_data = realloc(vec->data, new_capacity * sizeof(uint8_t));

        if (!new_data) return false;

        vec->data = new_data;
        vec->capacity = new_capacity;
    }

    vec->data[vec->size++] = value;

    return true;
}

bool vec_pop(Vec *vec, uint8_t *value) {
    if (!vec || !vec->data || vec->size == 0) return false;

    vec->size--;
    if (value) *value = vec->data[vec->size];

    return true;
}

bool vec_get(const Vec *vec, size_t index, uint8_t *value) {
    if (!vec || !vec->data || !value) return false;
    if (index >= vec->size) return false;

    *value = vec->data[index];

    return true;
}

bool vec_set(Vec *vec, size_t index, uint8_t value) {
    if (!vec || !vec->data) return false;
    if (index >= vec->size) return false;

    vec->data[index] = value;

    return true;
}
