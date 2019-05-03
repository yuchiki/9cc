#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "9cc.h"

//////////////////////// vector

Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
    return vec;
}

void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
    vec->data[vec->len++] = elem;
}

////////////////////////// map

Map *new_map() {
    Map *map = malloc(sizeof(Map));
    map->keys = new_vector();
    map->vals = new_vector();
    return map;
}

void map_put(Map *map, char *key, void *val) {
    vec_push(map->keys, key);
    vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
    for (int i = map->keys->len - 1; i >= 0; i--) {
        if (strcmp(map->keys->data[i], key) == 0) {
            return map->vals->data[i];
        }
    }

    return NULL;
}

///////////////////////// test

void expect(int line, int expected, int actual) {
    if (expected == actual) return;
    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void vector_test() {
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (long i = 0; i < 100; i++) vec_push(vec, (void *)i);

    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0, (long)vec->data[0]);
    expect(__LINE__, 50, (long)vec->data[50]);
    expect(__LINE__, 99, (long)vec->data[99]);
}

void map_test() {
    Map *map = new_map();

    expect(__LINE__, 0, (long)map->keys->len);
    expect(__LINE__, 0, (long)map_get(map, "foo"));

    map_put(map, "foo", (void *)2);
    expect(__LINE__, 2, (long)map_get(map, "foo"));
    expect(__LINE__, 1, (long)map->keys->len);

    map_put(map, "bar", (void *)4);
    expect(__LINE__, 4, (long)map_get(map, "bar"));
    expect(__LINE__, 2, (long)map_get(map, "foo"));
    expect(__LINE__, 2, (long)map->keys->len);

    map_put(map, "foo", (void *)6);
    expect(__LINE__, 6, (long)map_get(map, "foo"));
}
void runtest() {
    vector_test();
    map_test();
    printf("OK\n");
}
