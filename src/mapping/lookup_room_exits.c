int32_t lookup_room_exits (int32_t pnum, int32_t xadj, int32_t yadj, int zadj) {
    int32_t rv = 0;
    int32_t xloc = get_player_coord(X_COORD_REQUEST, pnum);
    int32_t yloc = get_player_coord(Y_COORD_REQUEST, pnum);
    int32_t zloc = get_player_coord(Z_COORD_REQUEST, pnum);

    uint8_t *sqlerr = NULL;
    uint8_t s_x[3];
    uint8_t s_y[3];
    uint8_t s_z[3];
    sprintf(s_x, "%d", xloc);
    sprintf(s_y, "%d", yloc);
    sprintf(s_z, "%d", zloc);

    uint8_t *room = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", s_x, s_y, s_z);
    int32_t rv = sqlite3_exec(get_roomdb(), room, callback, 0, &sqlerr); 
    sqlite3_free(room);
    if (rv != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(sqlerr);
        return -3;
    }
    if (sqlite_rows_count == 0) {
        return -2;
    }

    if (xadj == -1 && yadj == 0) { // west
        if (map.west == 0) {
            return -1; 
        }
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
    return 0;
}
