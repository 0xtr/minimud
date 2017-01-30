static int32_t adjusting_room_exit (const int32_t pnum, const _Bool reverse, const int32_t x, const int32_t y, const int32_t z);
static int32_t get_direction_as_number (const uint8_t *dir);

int32_t adjust_room_details (const int32_t adjusting, const _Bool reverse, const int32_t pnum, const int32_t x, const int32_t y, const int32_t z) {
    #define LONGEST_DIR 10
    uint8_t *sqlerr = NULL;
    uint8_t *room   = NULL;

    if (compare_room_owner(pnum, x, y, z) == -1) {
        return -3;
    }

    if (adjusting == ADJUSTING_ROOM_DESC) {
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rdesc = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                get_player_store(pnum), get_player_pname(pnum), (char)x, (char)y, (char)z);
    } else if (adjusting == ADJUSTING_ROOM_NAME) {
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rname = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                get_player_store(pnum), get_player_pname(pnum), (char)x, (char)y, (char)z);
    } else if (adjusting == ADJUSTING_ROOM_EXIT) {
        // room = adjusting_room_exit(pnum);
    } else if (adjusting == ADJUSTING_ROOM_FLAG) {
        // got to get current, then add to rflags
        // room = adjusting_room_flag(pnum);
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rflags = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                get_player_pname(pnum), (char)x, (char)y, (char)z);
    }

    lookup_room_exits(x, y, z, -1);
    if (sqlite3_exec(get_roomdb(), (char*)room, (char*)callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(room);
    return EXIT_SUCCESS;
}

const uint8_t VALID_DIRECTIONS[10][10] = {
    "north", "south", "west", "east", "northeast", "southwest", "northwest",
    "southeast", "up", "down"
};
int32_t get_opposite_dir (const uint8_t *dir) {
    size_t i;
    _Bool found = 0;
    for (i = 0; i < 10; ++i) {
        if (strcmp((char*)VALID_DIRECTIONS[i], (char*)dir) == 0) {
            found = true;
            break;
        }
    }
    if (found == false) {
        return EXIT_FAILURE;
    }
    if (strcmp((char*)dir, "south") == 0     ||
        strcmp((char*)dir, "east") == 0      ||
        strcmp((char*)dir, "southwest") == 0 ||
        strcmp((char*)dir, "southeast") == 0 ||
        strcmp((char*)dir, "down") == 0) {
        return i - 1;
    } else {
        return i + 1;
    }
    return EXIT_FAILURE;
}

#define NORTH_DIR       -9000
#define SOUTH_DIR       -9001
#define EAST_DIR        -9002
#define WEST_DIR        -9003
#define UP_DIR          -9004
#define DOWN_DIR        -9005
#define NORTHEAST_DIR   -9006
#define SOUTHEAST_DIR   -9007
#define SOUTHWEST_DIR   -9008
#define NORTHWEST_DIR   -9009
static int32_t get_direction_as_number (const uint8_t *dir) {
    if (strcmp((char*)dir, "north") || strcmp((char*)dir,"North")) {
        return NORTH_DIR;
    }
    if (strcmp((char*)dir, "south") || strcmp((char*)dir,"South")) {
        return SOUTH_DIR;
    }
    if (strcmp((char*)dir, "east") || strcmp((char*)dir,"East")) {
        return EAST_DIR;
    }
    if (strcmp((char*)dir, "west") || strcmp((char*)dir,"West")) {
        return WEST_DIR;
    }
    if (strcmp((char*)dir, "up") || strcmp((char*)dir,"Up")) {
        return UP_DIR;
    }
    if (strcmp((char*)dir, "down") || strcmp((char*)dir,"Down")) {
        return DOWN_DIR;
    }
    if (strcmp((char*)dir, "northeast") || strcmp((char*)dir,"Northeast")) {
        return NORTHEAST_DIR;
    }
    if (strcmp((char*)dir, "southeast") || strcmp((char*)dir,"Southeast")) {
        return SOUTHEAST_DIR;
    }
    if (strcmp((char*)dir, "southwest") || strcmp((char*)dir,"Southwest")) {
        return SOUTHWEST_DIR;
    }
    if (strcmp((char*)dir, "northwest") || strcmp((char*)dir,"Northwest")) {
        return NORTHWEST_DIR;
    }
    return EXIT_FAILURE;
}

static int32_t adjusting_room_exit (const int32_t pnum, const _Bool reverse, const int32_t x, const int32_t y, const int32_t z) {
    if (strlen((char*)get_player_store(pnum)) == 0 || get_player_store(pnum) == NULL) {
        return -1;
    }
    const int32_t direction = get_direction_as_number(get_player_store(pnum));
    assert(direction != 0);
    Map *map = lookup_room(x, y, z, pnum);
    uint8_t *room = sqlite3_mprintf("UPDATE CORE_ROOMS SET %Q = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
            get_dir_string(direction), reverse_of_current(direction, reverse), get_player_pname(pnum), (char)x, (char)y, (char)z);
    free_room(map);
    // stuff
    return EXIT_SUCCESS;
}
