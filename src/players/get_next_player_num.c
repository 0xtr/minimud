int32_t get_next_player_num (void) {
    int32_t status = 0;
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS;");
    uint8_t *sqlerr = NULL;
    sqlite_rows_count = 0;
    status = sqlite3_exec(playerdb, pcheck, callback, 0, &sqlerr); 
    if (status != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error in get_next_player_num:\n%s\n", sqlite3_errmsg(playerdb));
        return -1;
    }
    sqlite3_free(pcheck);
    return get_sqlite_rows_count() + 1;
}
