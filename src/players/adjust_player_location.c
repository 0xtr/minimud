int32_t adjust_player_location (const int32_t pnum, int32_t x, int32_t y, int32_t z) {
    if (x == -1 && y == -1 && z == -1) {
        x = y = z = 0;
    }
    uint8_t s_xloc[3];
    uint8_t s_yloc[3];
    uint8_t s_zloc[3];
    sprintf((char*)s_xloc, "%d", x);
    sprintf((char*)s_yloc, "%d", y);
    sprintf((char*)s_zloc, "%d", z);
    uint8_t *sqlerr = NULL;
    uint8_t *querystr = sqlite3_mprintf("UPDATE PLAYERS SET x = %Q, y = %Q, z = %Q WHERE pname = %Q;", 
                        s_xloc, s_yloc, s_zloc, get_player_pname(pnum));
    if (sqlite3_exec(get_playerdb(), (char*)querystr, callback, 0, &sqlerr) != SQLITE_OK) {
       fprintf(stdout, "SQLITE player location adjustment error:\n%s\n", sqlite3_errmsg(get_playerdb()));
       print_output(pnum, INVALDIR);
       sqlite3_free(querystr);
       sqlite3_free(sqlerr);
       return EXIT_FAILURE;
    }
    sqlite3_free(querystr);
    return EXIT_SUCCESS;
}
