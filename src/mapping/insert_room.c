int32_t insert_room (const uint8_t *rname, const int32_t xloc, const int32_t yloc, const int32_t zloc, const uint8_t *rdesc,
                     const uint8_t *owner, const uint8_t *flags) {
    int32_t room_total, status = 0;
    uint8_t *sqlerr = NULL;

    uint8_t *check = sqlite3_mprintf("SELECT * FROM CORE_ROOMS;");
    if (sqlite3_exec(get_roomdb(), (char*)check, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in insert_room SELECT:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        sqlite3_free(check);
        return EXIT_FAILURE;
    }
    sqlite3_free(check);
    room_total = get_sqlite_rows_count();

    // TODO: USE LOOKUP_ROOM AGAIN WITH MAP
    // 
    // check for rooms with the same coords
    check = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char)xloc, (char)yloc, (char)zloc);
    if (sqlite3_exec(get_roomdb(), check, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in insert_room:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(check);
        return -1;
    }
    sqlite3_free(check);
    if (get_sqlite_rows_count() != 0) {
        return -2;
    }

    uint8_t *querystr = sqlite3_mprintf("INSERT INTO CORE_ROOMS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
            (char)room_total+1, (char*)rname, (char*)rdesc, (char)xloc, (char)yloc, (char)zloc, 
            "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", 
            owner, owner, flags);
    if (sqlite3_exec(get_roomdb(), (char*)querystr, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room_insert error!\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(querystr);
    return EXIT_SUCCESS;
}
