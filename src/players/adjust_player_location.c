int32_t adjust_player_location (int32_t pnum, int32_t x, int32_t y, int32_t z) {
    uint8_t pname[NAMES_MAX] = {0};
    if (strlen(player[pnum].pname) > 0) {
        strncpy(pname, player[pnum].pname, NAMES_MAX);
    } else {
       fprintf(stdout, "An attempt was made to adjust a player's location, but couldn't find their name: %d.\n", pnum);
       return -1;
    }
    if (x == -1 && y == -1 && z == -1) {
        x = y = z = 0;
    }
    uint8_t s_xloc[3];
    uint8_t s_yloc[3];
    uint8_t s_zloc[3];
    sprintf(s_xloc, "%d", x);
    sprintf(s_yloc, "%d", y);
    sprintf(s_zloc, "%d", z);
    uint8_t *sqlerr = NULL;
    uint8_t *querystr = sqlite3_mprintf("UPDATE PLAYERS SET x = %Q, y = %Q, z = %Q WHERE pname = %Q;", 
                        s_xloc, s_yloc, s_zloc, pname);
    int32_t status = sqlite3_exec(playerdb, querystr, callback, 0, &sqlerr); 
    sqlite3_free(querystr);
    if (status != SQLITE_OK) {
       fprintf(stdout, "SQLITE player location adjustment error:\n%s\n", sqlite3_errmsg(playerdb));
       print_output(player[pnum].socknum, INVALDIR);
       sqlite3_free(sqlerr);
       return -1;
    }
    return 1;
}
