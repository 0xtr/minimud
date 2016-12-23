int32_t insert_room (const uint8_t *rname, const int32_t xloc, const int32_t yloc, const int32_t zloc, const uint8_t *rdesc,
                     const uint8_t *owner, const uint8_t *flags) {
    if (get_roomdb() == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    int32_t room_total, status = 0;
    uint8_t *sqlerr = NULL;

    uint8_t *check = sqlite3_mprintf("SELECT * FROM CORE_ROOMS;");
    status = sqlite3_exec(get_roomdb(), check, callback, 0, &sqlerr); 
    sqlite3_free(check);
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in insert_room SELECT:\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        return -1;
    }
    room_total = get_sqlite_rows_count();

    // get the room coords into a string
    uint8_t s_xloc[3];
    uint8_t s_yloc[3];
    uint8_t s_zloc[3];
    uint8_t rid[3];
    sprintf(s_xloc, "%d", xloc);
    sprintf(s_yloc, "%d", yloc);
    sprintf(s_zloc, "%d", zloc);
    sprintf(rid, "%d", room_total + 1);

    // check for rooms with the same coords
    check = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_xloc, s_yloc, s_zloc);
    status = sqlite3_exec(get_roomdb(), check, callback, 0, (char*)sqlerr); 
    sqlite3_free(check);
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 failure in insert_room:\n%s\n", sqlite3_errmsg(get_roomdb()));
        return -1;
    }
    if (sqlite_rows_count != 0) {
        return -2;
    }

    uint8_t *querystr = sqlite3_mprintf("INSERT INTO CORE_ROOMS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
            rid, rname, rdesc, 
            s_xloc, s_yloc, s_zloc, 
            "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", 
            owner, owner, flags);
    status = sqlite3_exec(get_roomdb(), (char*)querystr, callback, 0, (char*)sqlerr); 
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room_insert error!\n%s\n", sqlite3_errmsg(get_roomdb()));
        sqlite3_free(sqlerr);
        return -1;
    }
    sqlite3_free(querystr);
    return 1;
}
