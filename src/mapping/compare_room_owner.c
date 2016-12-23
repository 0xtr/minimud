int compare_room_owner (int pnum, char x[], char y[], char z[]) {
    int   rv;
    char  *room     = sqlite3_mprintf("SELECT * FROM CORE_ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", x, y, z);
    char  *sqlerr   = NULL;
    reset_sqlite_vars();
    rv = sqlite3_exec(db, room, callback, 0, &sqlerr); 
    if (rv != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 room owner lookup error:\n%s\n", sqlite3_errmsg(db));
        sqlite3_free(sqlerr);
        sqlite3_free(room);
        return -1;
    }
    sqlite3_free(room);
    if (sqlite_rows_count == 0) {
        return -1;
    }
    if (strcmp(player[pnum].pname, map.owner) == 0) {
        return 1;
    } else {
        return -1;
    }
    return 0;
}
