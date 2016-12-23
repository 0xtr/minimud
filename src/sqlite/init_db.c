int32_t init_db (const int DB_TYPE) {
    const *char DB_LOC = (DB_TYPE == -1) ? SQLITE_ROOMDB_LOC : SQLITE_PLAYERDB_LOC;
    int32_t status, tables_needed = 0;
    if (access(DB_LOC, F_OK) == -1) {
        tables_needed = 1;
    }
    sqlite3 *db = (DB_TYPE == -1) ? get_roomdb() : get_playerdb();
    status = sqlite3_open(DB_LOC, &playerdb);
    fprintf(stdout, "%s: %d\n", "SQLITE3 playerdb open status (0 success)", status);

    if (tables_needed == 1) {
        status = sqlite3_exec(get_playerdb(), 
                "CREATE TABLE PLAYERS (pnum INTEGER PRIMARY KEY," 
                "pname  TEXT, hash TEXT," 
                "salt   TEXT,"
                "last_ip TEXT,"
                "x INT, y INT, z INT)",
                callback, 0, NULL); 
        status = sqlite3_exec(get_roomdb(), 
                "CREATE TABLE CORE_ROOMS (id INTEGER PRIMARY KEY," 
                "rname TEXT, rdesc TEXT," 
                "xloc  INTEGER, yloc INT, zloc  INT,"
                "north INTEGER, east INT, south INT,"
                "west  INTEGER, up   INT, down  INT,"
                "northeast INTEGER, southeast INT, southwest INT,"
                "northwest INTEGER,"
                "owner TEXT,"
                "last_modified_by TEXT,"
                "flags TEXT)",
                callback, 0, NULL); 
        if (status != SQLITE_OK) {
           fprintf(stdout, "SQLITE3 player database setup error! Closing the database:\n%s\n", sqlite3_errmsg(playerdb));
           exit(EXIT_FAILURE);
        }
    }
    // insert_base_room_if_not_existing();
    // check that we have at least the origin room
    status = insert_room(BASE_ROOM_NAME, 0, 0, 0, BASE_ROOM_DESC, (uint8_t*)"admin", (uint8_t*)"none");

    return 1;
}

void reset_player_sqlite_vars (void) {
    player_tmp.x = 0;
    player_tmp.y = 0;
    player_tmp.z = 0;
}
