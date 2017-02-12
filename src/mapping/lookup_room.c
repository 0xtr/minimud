Map *lookup_room (const int32_t x, const int32_t y, const int32_t z, const int32_t pnum) {
    uint8_t *sqlerr = NULL;
    Map *map = get_room();
    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char)x, (char)y, (char)z);

    if (sqlite3_exec(get_roomdb(), (char*)room, callback, map, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        free(map);
        return NULL;
    }

    sqlite3_free(room);

    if (pnum == -1)
    	    return map;

    set_player_buffer_replace(pnum, (uint8_t*)"> ");
    if (get_sqlite_rows_count() == 0) {
        set_player_buffer_append(pnum, (uint8_t*)"NULL SPACE");
    } else {
        set_player_buffer_append(pnum, map->rname);
    }

    assert(outgoing_msg_handler(pnum) == EXIT_SUCCESS);

    set_player_buffer_replace(pnum, (uint8_t*)"[");
    // room x
    set_player_buffer_append(pnum, (uint8_t)x);
    set_player_buffer_append(pnum, (uint8_t*)"][");
    // room y
    set_player_buffer_append(pnum, (uint8_t)y);
    set_player_buffer_append(pnum, (uint8_t*)"][");
    // room z
    set_player_buffer_append(pnum, (uint8_t)z);
    set_player_buffer_append(pnum, (uint8_t*)"]");

    assert(outgoing_msg_handler(pnum) == EXIT_SUCCESS);

    if (map == NULL) {
        set_player_buffer_replace(pnum, (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.");
    } else {
        set_player_buffer_replace(pnum, map->rdesc);
    }
    assert(outgoing_msg_handler(pnum) == EXIT_SUCCESS);

    return map;
}

int32_t lookup_room_exits (const int32_t pnum, const int32_t xadj, const int32_t yadj, const int32_t zadj) {
    Map *map = lookup_room(get_player_coord(X_COORD_REQUEST, pnum), 
                           get_player_coord(X_COORD_REQUEST, pnum), 
                           get_player_coord(X_COORD_REQUEST, pnum), -1);
    if (map == NULL) {
        return -2;
    }
    if (!has_exit_for_dir(xadj, yadj, zadj, map)) {
        free_room(map);
        return -1;
    }
    free_room(map);
    return EXIT_SUCCESS;
}

_Bool is_vector_west (const int32_t x, const int32_t y) {
    return x == -1 && y == 0;
}

_Bool has_west_exit (const Map *map) {
    if (map->west == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
    //return map.west == 0;
}

int32_t has_exit_for_dir (const int32_t x, const int32_t y, const int32_t z, const Map *map) {
    if (is_vector_west(x, y) && !has_west_exit(map)) {
        return EXIT_FAILURE;
    }
    if (x == 1 && y == 0) { // east
        if (map->east == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == 0 && y == 1) { // north
        if (map->north == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == 0 && y == -1) { // south
        if (map->south == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == 1 && y == 1) { // northeast
        if (map->northeast == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == 1 && y == -1) { // southeast
        if (map->southeast == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == -1 && y == -1) { // southwest
        if (map->southwest == 0) {
            return EXIT_FAILURE;
        }
    }
    if (x == -1 && y == 1) { // northwest
        if (map->northwest == 0) {
            return EXIT_FAILURE;
        }
    }
    if (z == 1) { // up
        if (map->up == 0) {
            return EXIT_FAILURE;
        }
    }
    if (z == -1) { // down
        if (map->down == 0) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int32_t lookup_room_name_from_coords (const int32_t pnum, const int32_t x, const int32_t y, const int32_t z) {
    Map *map = lookup_room(x, y, z, -1);
    if (map != NULL) {
        // get_room_details(map);
        set_player_buffer_replace(pnum, map->rname);
        free_room(map);
    } else {
        set_player_buffer_replace(pnum, (uint8_t*)"an unknown location");
    }
    return EXIT_SUCCESS;
}

int32_t compare_room_owner (const int32_t pnum, const uint8_t *x, const uint8_t *y, const uint8_t *z) {
    Map *map = lookup_room (x, y, z, -1);
    if (map == NULL) {
        return -1;
    }
    if (strcmp((char*)get_player_pname(pnum), map->owner) != 0) {
        free_room(map);
        return EXIT_FAILURE;
    }
    free_room(map);
    return EXIT_SUCCESS;
}
