#pragma once

struct Map {
    uint8_t rname[NAMES_MAX];
    uint8_t rdesc[BUFFER_LENGTH];
    uint8_t owner[NAMES_MAX];
    uint8_t last_modified[NAMES_MAX];
    _Bool north;
    _Bool south;
    _Bool west;
    _Bool east;
    _Bool down;
    _Bool up;
    _Bool northeast;
    _Bool southeast;
    _Bool southwest;
    _Bool northwest;
}; 
struct Map *get_room(void);
void free_room(struct Map *map);
