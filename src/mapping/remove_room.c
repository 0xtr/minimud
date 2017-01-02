int32_t remove_room (int32_t x, int32_t y, int32_t z, int32_t socknum) {
    uint8_t *sqlerr = NULL;

    // get the room coords into a string
    if (x == -1 && y == -1 && z == -1) {
        x = get_player_coord(X_COORD_REQUEST, pnum);
        y = get_player_coord(Y_COORD_REQUEST, pnum);
        z = get_player_coord(Z_COORD_REQUEST, pnum);
    }
    uint8_t xloc[3];
    uint8_t yloc[3];
    uint8_t zloc[3];
    sprintf(xloc, "%d", x);
    sprintf(yloc, "%d", y);
    sprintf(zloc, "%d", z);

    uint8_t *check = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", xloc, yloc, zloc);
    int32_t status = sqlite3_exec(db, check, callback, 0, &sqlerr); 
    sqlite3_free(check);
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in remove_room; could not find the room:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(sqlerr);
        return -3;
    }
    if (get_sqlite_rows_count() == 0) {
        return -1;
    }
    if (strcmp(map.owner, player[pnum].pname) != 0) {
        return -2;
    }

    uint8_t *querystr = sqlite3_mprintf("DELETE FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", xloc, yloc, zloc);
    status = sqlite3_exec(db, querystr, callback, 0, &sqlerr); 
    sqlite3_free(querystr);
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in remove_room; could not delete the room:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(sqlerr);
        return -3;
    }

    uint8_t unlinkdirs[LONGEST_DIR];
    memset(unlinkdirs, '\0', LONGEST_DIR);
    if (map.north == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -2, x, y + 1, z);
    }
    if (map.south == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -1, x, y - 1, z);
    }
    if (map.east == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -4, x + 1, y, z);
    }
    if (map.west == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -3, x + 1, y, z);
    }

    if (map.up == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -6, x, y, z + 1);
    }
    if (map.down == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -5, x, y - 1, z);
    }

    if (map.northeast == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -9, x + 1, y + 1, z);
    }
    if (map.southeast == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -10, x + 1, y - 1, z);
    }
    if (map.southwest == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -7, x - 1, y - 1, z);
    }
    if (map.northwest == 1) {
        status = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -8, x - 1, y + 1, z);
    }
    //status = check_exits_and_adjust(ADJUSTING_ROOM_EXIT, x, y, z);

    remove_players_from_room(x, y, z);
    return 1;
}

int32_t remove_players_from_room (int32_t x, int32_t y, int32_t z) {
    for (size_t i = 0; i != MAX_CONNS && i < get_active_conns(); ++i) {
        if (player[i].in_use == 1) {
            if (get_player_coord(X_COORD_REQUEST, i) == x &&
                get_player_coord(Y_COORD_REQUEST, i) == y &&
                get_player_coord(Z_COORD_REQUEST, i) == z) {
                print_output(player[i].socknum, PRINT_REMOVED_FROM_ROOM);
                adjust_player_location(i, -1, -1, -1);
                print_output(player[i].socknum, SHOWROOM);
            }
        }
    }
    return 0;
}
