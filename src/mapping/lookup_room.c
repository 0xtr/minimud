int32_t lookup_room (int32_t x, int32_t y, int32_t z, int32_t socknum) {
    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    uint8_t coords[10] = {0};
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
    if (socknum == -1) {
        if (get_sqlite_rows_count() == 0) {
            return -1;
        }
        return 0;
    }
    strcpy(buf, "> ");
    if (sqlite_rows_count == 0) {
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

    if (sqlite_rows_count == 0) {
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

    return 0;
}
