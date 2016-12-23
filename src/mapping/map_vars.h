#pragma once

uint8_t *ptr = NULL;
FILE *fp_room;
struct {
    uint8_t rname[NAMES_MAX];
    uint8_t rdesc[BUFLEN];
    uint8_t owner[NAMES_MAX];
    uint8_t last_modified[NAMES_MAX];
    _Bool is_occupied;
    int32_t north;
    int32_t south;
    int32_t west;
    int32_t east;
    int32_t down;
    int32_t up;
    int32_t northeast;
    int32_t southeast;
    int32_t southwest;
    int32_t northwest;
} map; 

#define VALID_DIRS 10
#define LONGEST_DIR 10 // dir + 1 null
const uint8_t valid_dirs[VALID_DIRS] = {
    "north", "south", "west", "east", "northeast", "southwest", "northwest",
    "southeast", "up", "down"
};
