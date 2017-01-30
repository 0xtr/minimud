int32_t lookup_player (const uint8_t *pname) {
    uint8_t *sqlerr = NULL;
    // check for players with the same name 
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", pname);
    if (sqlite3_exec(get_playerdb(), (char*)pcheck, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error in lookup_player:\n%s\n", sqlite3_errmsg(get_playerdb()));
        sqlite3_free(pcheck);
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(pcheck);
    return (get_sqlite_rows_count() != 0) ? 1 : 0;
}
