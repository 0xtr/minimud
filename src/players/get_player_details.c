int32_t get_existing_player_hash (const int32_t pnum) {
    uint8_t *sqlerr = NULL;
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", get_player_pname(pnum));
    if (sqlite3_exec(get_playerdb(), (char*)pcheck, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        fprintf(stdout, "SQLITE3 error! Failed to get player's hash:\n%s\n", sqlite3_errmsg(get_playerdb()));
        sqlite3_free(sqlerr);
        sqlite3_free(pcheck);
        return EXIT_FAILURE;
    }
    sqlite3_free(pcheck);
    if (get_sqlite_rows_count() == 0) {
        fprintf(stdout, "Couldn't retrieve the hash for player %d.\n", pnum);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int32_t get_player_coord (const int32_t coord_type, const int32_t pnum) {
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS WHERE pname LIKE %Q;", get_player_pname(pnum));
    uint8_t *sqlerr = NULL;
    if (sqlite3_exec(get_playerdb(), (char*)pcheck, callback, 0, (char**)sqlerr) != SQLITE_OK) {
#ifdef DEBUG
        fprintf(stdout, "SQLITE3 error in get_player_coord:\n%s\n", sqlite3_errmsg(get_playerdb()));
#endif
        sqlite3_free(pcheck);
        sqlite3_free(sqlerr);
        return EXIT_FAILURE;
    }
    sqlite3_free(pcheck);
    if (coord_type == X_COORD_REQUEST) {
        //return player_tmp.x;
    } else if (coord_type == Y_COORD_REQUEST) {
        //return player_tmp.y;
    } else if (coord_type == Z_COORD_REQUEST) {
        //return player_tmp.z;
    }
    return EXIT_FAILURE;
}

int32_t get_next_player_num (void) {
    uint8_t *pcheck = sqlite3_mprintf("SELECT * FROM PLAYERS;");
    uint8_t *sqlerr = NULL;
    if (sqlite3_exec(get_playerdb(), (char*)pcheck, callback, 0, (char**)sqlerr) != SQLITE_OK) {
        sqlite3_free(pcheck);
        sqlite3_free(sqlerr);
#ifdef DEBUG
        fprintf(stdout, "SQLITE3 error in get_next_player_num:\n%s\n", sqlite3_errmsg(get_playerdb()));
#endif
        return EXIT_FAILURE;
    }
    sqlite3_free(pcheck);
    return get_sqlite_rows_count() + 1;
}
