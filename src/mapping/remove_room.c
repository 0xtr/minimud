int32_t remove_room (int32_t x, int32_t y, int32_t z, const int32_t pnum) {
    uint8_t *sqlerr = NULL;

    if (x == -1 && y == -1 && z == -1) {
        x = get_player_coord(X_COORD_REQUEST, pnum);
        y = get_player_coord(Y_COORD_REQUEST, pnum);
        z = get_player_coord(Z_COORD_REQUEST, pnum);
    }

    Map *map = lookup_room(x, y, z, pnum);
    if (map == NULL) {
        free_room(map);
        return -1;
    }
    if (strcmp((char*)map->owner, (char*)get_player_pname(pnum)) != 0) {
        free_room(map);
        return -2;
    }

    uint8_t *querystr = sqlite3_mprintf("DELETE FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char)x, (char)y, (char)z);
    if (sqlite3_exec(get_roomdb(), (char*)querystr, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in remove_room; could not delete the room:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(querystr);
        sqlite3_free(sqlerr);
        free_room(map);
        return -3;
    }
    sqlite3_free(querystr);

    if (map->north == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -2, x, y + 1, z);
    }
    if (map->south == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -1, x, y - 1, z);
    }
    if (map->east == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -4, x + 1, y, z);
    }
    if (map->west == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -3, x + 1, y, z);
    }

    if (map->up == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -6, x, y, z + 1);
    }
    if (map->down == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -5, x, y - 1, z);
    }

    if (map->northeast == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -9, x + 1, y + 1, z);
    }
    if (map->southeast == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -10, x + 1, y - 1, z);
    }
    if (map->southwest == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -7, x - 1, y - 1, z);
    }
    if (map->northwest == 1) {
        adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -8, x - 1, y + 1, z);
    }
    //status = check_exits_and_adjust(ADJUSTING_ROOM_EXIT, x, y, z);

    assert(remove_players_from_room(x, y, z) == EXIT_SUCCESS);
    free_room(map);
    return EXIT_SUCCESS;
}

int32_t remove_players_from_room (const int32_t x, const int32_t y, const int32_t z) {
    for (size_t i = 0; i < get_num_of_players(); ++i) {
        if (get_player_in_use(i) == 1) {
            if (get_player_coord(X_COORD_REQUEST, i) == x &&
                get_player_coord(Y_COORD_REQUEST, i) == y &&
                get_player_coord(Z_COORD_REQUEST, i) == z) {
                print_output(i, PRINT_REMOVED_FROM_ROOM);
                adjust_player_location(i, -1, -1, -1);
                print_output(i, SHOWROOM);
            }
        }
    }
    return EXIT_SUCCESS;
}
