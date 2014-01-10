#include <stdlib.h>
#include "map.h"

/* fast hash function with few collisions
   http://stackoverflow.com/a/10832203/1574697 */
int hash_int(int k) {
    k *= 357913941;
    k ^= k << 24;
    k += ~357913941;
    k ^= k >> 31;
    k ^= k << 31;
    return k;
}

int hash(int x, int y, int z) {
    x = hash_int(x);
    y = hash_int(y);
    z = hash_int(z);
    return x ^ y ^ z;
}

void map_alloc(Map *map) {
    map->mask = 0xfff;
    map->size = 0;
    map->data = (MapEntry *)calloc(map->mask + 1, sizeof(MapEntry));
}

void map_free(Map *map) {
    free(map->data);
}

int map_set(Map *map, int x, int y, int z, int w) {
    unsigned int index = hash(x, y, z) & map->mask;
    MapEntry *entry = map->data + index;
    int overwrite = 0;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->x == x && entry->y == y && entry->z == z) {
            overwrite = 1;
            break;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
    }
    if (overwrite) {
        if (entry->w != w) {
            entry->w = w;
            return 1;
        }
    }
    else if (w) {
        entry->x = x;
        entry->y = y;
        entry->z = z;
        entry->w = w;
        map->size++;
        if (map->size * 2 > map->mask) {
            map_grow(map);
        }
        return 1;
    }
    return 0;
}

int map_get(Map *map, int x, int y, int z) {
    unsigned int index = hash(x, y, z) & map->mask;
    MapEntry *entry = map->data + index;
    while (!EMPTY_ENTRY(entry)) {
        if (entry->x == x && entry->y == y && entry->z == z) {
            return entry->w;
        }
        index = (index + 1) & map->mask;
        entry = map->data + index;
    }
    return 0;
}

void map_grow(Map *map) {
    Map new_map;
    new_map.mask = (map->mask << 1) | 1;
    new_map.size = 0;
    new_map.data = (MapEntry *)calloc(new_map.mask + 1, sizeof(MapEntry));
    MAP_FOR_EACH(map, entry) {
        map_set(&new_map, entry->x, entry->y, entry->z, entry->w);
    } END_MAP_FOR_EACH;
    free(map->data);
    map->mask = new_map.mask;
    map->size = new_map.size;
    map->data = new_map.data;
}
