int32_t get_player_coord (int32_t coord_type, int32_t pnum) {
    reset_player_sqlite_vars();
    int32_t status = 0;
    if (playerdb == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No player database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", player[pnum].pname);
    uint8_t *sqlerr = NULL;

    int32_t sqlite_rows_count = 0;
    status = sqlite3_exec(playerdb, pcheck, callback, 0, &sqlerr); 
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error in get_player_coord:\n%s\n", sqlite3_errmsg(playerdb));
        return -1;
    }
    sqlite3_free(pcheck);
    return (coord_type == X_COORD_REQUEST) ? player_tmp.x : (coord_type == Y_COORD_REQUEST) ? player_tmp.y : player_tmp.z;
    /*
    if (coord_type == X_COORD_REQUEST) {
        return player_tmp.x;
    } else if (coord_type == Y_COORD_REQUEST) {
        return player_tmp.y;
    } else if (coord_type == Z_COORD_REQUEST) {
        return player_tmp.z;
    }
    */
}
