int32_t lookup_room_name_from_coords (int32_t x, int32_t y, int32_t z) {
    if (get_roomdb() == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No room database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    sprintf(s_x, "%d", x);
    sprintf(s_y, "%d", y);
    sprintf(s_z, "%d", z);

    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    int32_t rv = sqlite3_exec(db, room, callback, 0, &sqlerr); 
    sqlite3_free(room);
    if (rv != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(sqlerr);
        return -1;
    }
    if (sqlite_rows_count != 0) {
        strcat(buf, (uint8_t*)map.rname);
    } else {
        strcat(buf, (uint8_t*)"an unknown location");
    }
    return 0;
}
