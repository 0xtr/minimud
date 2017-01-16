Map *lookup_room (const int32_t x, const int32_t y, const int32_t z, const int32_t pnum) {
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    uint8_t coords[10] = {0};
    sprintf(s_x, "%d", x);
    sprintf(s_y, "%d", y);
    sprintf(s_z, "%d", z);

    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    if (sqlite3_exec(get_roomdb(), (char*)room, callback, 0, &sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(room);
    Map *map = get_room();
    // wut
    //if (socknum == -1) {
    /*
        if (get_sqlite_rows_count() == 0) {
            return EXIT_FAILURE;
        }
        //return EXIT_SUCCESS;
        */
    //}
    set_player_buffer_replace(pnum, "> ");
    if (get_sqlite_rows_count() == 0) {
        set_player_buffer_append(pnum, (uint8_t*)"NULL SPACE");
    } else {
        set_player_buffer_append(pnum, map->rname);
    }

    // filter_forbidden_chars(get_player_buffer(pnum));
    /*
    if (buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    } 
    if (strrchr(buf, '\r') != NULL) {
        for (size_t i = 0; i != NAMES_MAX; ++i) {
            if (buf[i] == '\r') {
                buf[i] = '\0';
                break;
            }
        }
    }
    */
    assert(send_and_ensure(pnum, NULL) == EXIT_SUCCESS);

    set_player_buffer_replace(pnum, (uint8_t*)"[");
    // room x
    set_player_buffer_append(pnum, (uint8_t*)x);
    set_player_buffer_append(pnum, (uint8_t*)"][");
    // room y
    set_player_buffer_append(pnum, (uint8_t*)y);
    set_player_buffer_append(pnum, (uint8_t*)"][");
    // room z
    set_player_buffer_append(pnum, (uint8_t*)z);
    set_player_buffer_append(pnum, (uint8_t*)"]");

    assert(send_and_ensure(pnum, NULL) == EXIT_SUCCESS);
    if (get_sqlite_rows_count() == 0) {
        set_player_buffer_replace(pnum, "It is pitch black. You are likely to be eaten by a null character.");
    } else {
        set_player_buffer_replace(pnum, map->rdesc);
    }
    /*
    if (buf[strlen(buf)] == '\n') {
        buf[strlen(buf)] = '\0';
    }
    if (buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    }
    */
    rv = send_and_ensure(pnum, NULL);

    return map;
}

int32_t lookup_room_exits (const int32_t pnum, const int32_t xadj, const int32_t yadj, const int32_t zadj) {
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    sprintf(s_x, "%d", get_player_coord(X_COORD_REQUEST, pnum));
    sprintf(s_y, "%d", get_player_coord(Y_COORD_REQUEST, pnum));
    sprintf(s_z, "%d", get_player_coord(Z_COORD_REQUEST, pnum));

    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    if (sqlite3_exec(get_roomdb(), (char*)room, callback, 0, &sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return -3;
    }
    sqlite3_free(room);
    if (get_sqlite_rows_count() == 0) {
        return -2;
    }
    if (!has_exit_for_dir(xadj, yadj, zadj)) {
        return -1;
    }
    return EXIT_SUCCESS;
}

_Bool is_vector_west (const int32_t x, const int32_t y) {
    return x == -1 && y == 0;
}

_Bool has_west_exit (const Map map) {
    if (map.west == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
    //return map.west == 0;
}

int32_t has_exit_for_dir (const int32_t x, const int32_t y, const int32_t z, const Map map) {
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
    uint8_t *sqlerr = NULL;
    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char*)x, (char*)y, (char*)z);
    if (sqlite3_exec(get_roomdb(), (char*)room, callback, 0, &sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(room);
    if (get_sqlite_rows_count() != 0) {
        Map map = get_room();
        // get_room_details(map);
        set_player_buffer_replace(pnum, map.rname);
        free(map);
    } else {
        set_player_buffer_replace(pnum, (uint8_t*)"an unknown location");
    }
    return EXIT_SUCCESS;
}
