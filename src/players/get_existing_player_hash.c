int32_t get_existing_player_hash (int32_t pnum) {
    if (get_playerdb() == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No player database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    int32_t status = 0;
    uint8_t *sqlerr = NULL;
    memset(player_tmp.hash, '\0', HASHSPACE);

    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", player[pnum].pname);
    sqlite_rows_count = 0;
    status = sqlite3_exec(playerdb, pcheck, callback, 0, &sqlerr); 
    sqlite3_free(pcheck);
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error! Failed to get player's hash:\n%s\n", sqlite3_errmsg(playerdb));
        sqlite3_free(sqlerr);
        return -1;
    }
    if (get_sqlite_rows_count() == 0) {
        fprintf(stdout, "Couldn't retrieve the hash for player %d.\n", pnum);
        return -1;
    }
    return EXIT_SUCCESS;
}
