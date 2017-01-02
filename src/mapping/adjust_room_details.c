const uint8_t valid_dirs[10] = {
    "north", "south", "west", "east", "northeast", "southwest", "northwest",
    "southeast", "up", "down"
};

int32_t adjust_room_details (int32_t adjusting, int32_t reverse, int32_t pnum, int32_t x, int32_t y, int32_t z) {
    #define SET_VAL \
        if (val1 == 0 && pnum != -1) {\
            snprintf(val, sizeof(int), "%d", 1);\
        } else {\
            snprintf(val, sizeof(int), "%d", 0);\
        }\

    int32_t rv = 0;
    uint8_t *sqlerr = NULL;
    uint8_t *room   = NULL;
    uint8_t newroom_x[3];
    uint8_t newroom_y[3];
    uint8_t newroom_z[3];
    sprintf(newroom_x, "%d", x);
    sprintf(newroom_y, "%d", y);
    sprintf(newroom_z, "%d", z);

    if (pnum >= 0) {
        if (adjusting != ADJUSTING_ROOM_EXIT) {
            if (compare_room_owner(pnum, newroom_x, newroom_y, newroom_z) == -1) {
                return -3;
            }
        }
    }
    if (adjusting == ADJUSTING_ROOM_DESC) {
        // room = adjusting_room_desc(pnum);
        uint8_t *roomdetails = player[pnum].store;
        uint8_t roomtmp[BUFLEN] = {0};
        strncpy(roomtmp, roomdetails, BUFLEN);
        if (strrchr(roomtmp, '\r') != NULL) {
            for (size_t i = 0; i != BUFLEN; ++i) {
                if (buf[i] == '\r') {
                    buf[i] = '\0';
                    break;
                }
            }
        }
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rdesc = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                roomtmp, player[pnum].pname, newroom_x, newroom_y, newroom_z);
    } else if (adjusting == ADJUSTING_ROOM_NAME) {
        // room = adjusting_room_name(pnum);
        uint8_t *roomdetails = player[pnum].store;
        uint8_t roomtmp[strlen(roomdetails)];
        strncpy(roomtmp, roomdetails, NAMES_MAX);
        if (strrchr(roomtmp, '\r') != NULL) {
            for (size_t i = 0; i != NAMES_MAX; ++i) {
                if (buf[i] == '\r') {
                    buf[i] = '\0';
                    break;
                }
            }
        }
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rname = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                roomtmp, player[pnum].pname, newroom_x, newroom_y, newroom_z);
    } else if (adjusting == ADJUSTING_ROOM_EXIT) {
        // room = adjusting_room_exit(pnum);
        if (pnum >= 0) {
            if (player[pnum].store_size == 0 || player[pnum].store == NULL) {
                return -1;
            }
        }
        int32_t val1 = 0;
        uint8_t val[3];
        memset(val, '\0', 3);

        uint8_t tmp[LONGEST_DIR];
        memset(tmp, '\0', LONGEST_DIR);
        if (pnum >= 0) {
            if (player[pnum].store_size > LONGEST_DIR) {
                return -1;
            }
            strncpy(tmp, player[pnum].store, player[pnum].store_size);
        } else {
            switch (pnum) {
                case -1:
                    strncpy(tmp, "north", LONGEST_DIR);
                    break;
                case -2:
                    strncpy(tmp, "south", LONGEST_DIR);
                    break;
                case -3:
                    strncpy(tmp, "east", LONGEST_DIR);
                    break;
                case -4:
                    strncpy(tmp, "west", LONGEST_DIR);
                    break;
                case -5:
                    strncpy(tmp, "up", LONGEST_DIR);
                    break;
                case -6:
                    strncpy(tmp, "down", LONGEST_DIR);
                    break;
                case -7:
                    strncpy(tmp, "northeast", LONGEST_DIR);
                    break;
                case -8:
                    strncpy(tmp, "southeast", LONGEST_DIR);
                    break;
                case -9:
                    strncpy(tmp, "southwest", LONGEST_DIR);
                    break;
                case -10:
                    strncpy(tmp, "northwest", LONGEST_DIR);
                    break;
                default:
                    fprintf(stdout, "Unhandled case for pnum < 0 in adjust_details room exit.\n");
                    return -1;
            }
        }
        if (reverse == 1) {
            strncpy(tmp, valid_dirs[get_opposite_dir(tmp)], LONGEST_DIR);
        }
        rv = lookup_room(x, y, z, -1);
        if (strcmp(tmp, "north") == 0) {
            val1 = map.north;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET north = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "east") == 0) {
            val1 = map.east;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET east = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "south") == 0) {
            val1 = map.south;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET south = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "west") == 0) {
            val1 = map.west;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET west = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "northeast") == 0) {
            val1 = map.northeast;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET northeast = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "southeast") == 0) {
            val1 = map.southeast;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET southeast = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "southwest") == 0) {
            val1 = map.southwest;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET southwest = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "northwest") == 0) {
            val1 = map.northwest;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET northwest = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "up") == 0) {
            val1 = map.up;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET up = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        } else if (strcmp(tmp, "down") == 0) {
            val1 = map.down;
            SET_VAL;
            room = sqlite3_mprintf("UPDATE CORE_ROOMS SET down = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                val, player[pnum].pname, newroom_x, newroom_y, newroom_z);
        }
    } else if (adjusting == ADJUSTING_ROOM_FLAG) {
        // got to get current, then add to rflags
        // room = adjusting_room_flag(pnum);
        room = sqlite3_mprintf("UPDATE CORE_ROOMS SET rflags = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
                player[pnum].pname, newroom_x, newroom_y, newroom_z);
    }
    if (room == NULL) {
        log_issue("Room query string not set - invalid option to adjust_room_details?");
        return -2;
    }
    //
    rv = sqlite3_exec(get_roomdb(), room, callback, 0, &sqlerr); 
    sqlite3_free(room);
    //
    lookup_room_exits(x, y, z, -1);
    if (rv != SQLITE_OK) {
        fprintf(stdout, "[ABORTING] SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        return -1;
    }
    return 1;
}

int32_t get_opposite_dir (const uint8_t *dir) {
    int32_t i, found = 0;
    for (i = 0; i != VALID_DIRS; ++i) {
        if (strcmp((char*)valid_dirs[i], (char*)dir) == 0) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        return -1;
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
    return -1;
}
