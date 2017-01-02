int32_t lookup_room (const int32_t x, const int32_t y, const int32_t z, const int32_t socknum) {
    const int32_t pnum = getplayernum(socknum);
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    uint8_t coords[10] = {0};
    sprintf(s_x, "%d", x);
    sprintf(s_y, "%d", y);
    sprintf(s_z, "%d", z);

    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    if (sqlite3_exec(get_roomdb(), room, callback, 0, &sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return -1;
    }
    sqlite3_free(room);
    if (socknum == -1) {
        if (get_sqlite_rows_count() == 0) {
            return -1;
        }
        return 0;
    }
    strcpy(get_player(pnum).buffer, "> ");
    if (get_sqlite_rows_count() == 0) {
        strncat(buf, "NULL SPACE", NAMES_MAX - strlen(buf));
    } else {
        strncat(buf, map.rname, NAMES_MAX - strlen(buf));
    }

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
    rv = send_and_ensure(socknum);

    strcpy(buf, "[");
    // room x
    snprintf(coords, sizeof(x), "%d", x);
    strcat(buf, coords);
    strcat(buf, "][");
    memset(coords, '\0', 10);
    // room y
    snprintf(coords, sizeof(y), "%d", y);
    strcat(buf, coords);
    strcat(buf, "][");
    memset(coords, '\0', 10);
    // room z
    snprintf(coords, sizeof(z), "%d", z);
    strcat(buf, coords);
    strcat(buf, "]");
    rv = send_and_ensure(socknum);

    if (get_sqlite_rows_count() == 0) {
        strcpy(buf, "It is pitch black. You are likely to be eaten by a null character.");
    } else {
        strncpy(buf, map.rdesc, BUFLEN - 2);
    }
    if (buf[strlen(buf)] == '\n') {
        buf[strlen(buf)] = '\0';
    }
    if (buf[strlen(buf) - 1] == '\n') {
        buf[strlen(buf) - 1] = '\0';
    }
    rv = send_and_ensure(socknum);

    return EXIT_SUCCESS;
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
    if (sqlite3_exec(&get_roomdb(), room, callback, 0, &sqlerr) != SQLITE_OK) {
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

_Bool has_west_exit (const Map_t map) {
    if (map.west == 0) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
    //return map.west == 0;
}

int32_t has_exit_for_dir (const int32_t x, const int32_t y, const int32_t z) {
    if (is_vector_west(xadj, yadj) && !has_west_exit(map)) {
        return EXIT_FAILURE;
    }
    if (xadj == 1 && yadj == 0) { // east
        if (map.east == 0) {
            return -1;
        }
    }
    if (xadj == 0 && yadj == 1) { // north
        if (map.north == 0) {
            return -1;
        }
    }
    if (xadj == 0 && yadj == -1) { // south
        if (map.south == 0) {
            return -1;
        }
    }
    if (xadj == 1 && yadj == 1) { // northeast
        if (map.northeast == 0) {
            return -1;
        }
    }
    if (xadj == 1 && yadj == -1) { // southeast
        if (map.southeast == 0) {
            return -1;
        }
    }
    if (xadj == -1 && yadj == -1) { // southwest
        if (map.southwest == 0) {
            return -1;
        }
    }
    if (xadj == -1 && yadj == 1) { // northwest
        if (map.northwest == 0) {
            return -1;
        }
    }
    if (zadj == 1) { // up
        if (map.up == 0) {
            return -1;
        }
    }
    if (zadj == -1) { // down
        if (map.down == 0) {
            return -1;
        }
    }
    return EXIT_SUCCESS;
}

int32_t lookup_room_name_from_coords (const int32_t x, const int32_t y, const int32_t z) {
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    sprintf(s_x, "%d", x);
    sprintf(s_y, "%d", y);
    sprintf(s_z, "%d", z);
    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    if (sqlite3_exec(get_roomdb(), room, callback, 0, &sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(room);
        sqlite3_free(sqlerr);
        return -1;
    }
    sqlite3_free(room);
    if (get_sqlite_rows_count() != 0) {
        strcat(buf, (uint8_t*)map.rname);
    } else {
        strcat(buf, (uint8_t*)"an unknown location");
    }
    return 0;
}
