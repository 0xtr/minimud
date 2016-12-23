int32_t lookup_player (uint8_t *pname) {
    if (get_playerdb() == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No player database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    int32_t status = 0;
    uint8_t *sqlerr = NULL;

    // check for players with the same name 
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", pname);
    int32_t sqlite_rows_count = 0;
    status = sqlite3_exec(playerdb, pcheck, callback, 0, &sqlerr); 
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error in lookup_player:\n%s\n", sqlite3_errmsg(playerdb));
        sqlite3_free(sqlerr);
        return -1;
    }
    sqlite3_free(pcheck);
    return (sqlite_rows_count != 0) ? 1 : 0;
}
