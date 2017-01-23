int32_t compare_room_owner (const int32_t pnum, const uint8_t *x, const uint8_t *y, const uint8_t *z) {
    uint8_t *room = sqlite3_mprint32_tf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", x, y, z);
    uint8_t *sqlerr = NULL;
    reset_sqlite_vars();
    if (sqlite3_exec(get_roomdb(), (char*)room, (char*)callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room owner lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        sqlite3_free(room);
        return EXIT_FAILURE;
    }
    sqlite3_free(room);
    if (get_sqlite_rows_count() == 0) {
        return -1;
    }
    if (strcmp((char*)get_player_pname(pnum), map.owner) != 0) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
