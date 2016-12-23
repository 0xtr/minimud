sqlite3 *get_roomdb (void) {
    return roomdb;
}

void *set_roomdb (sqlite3 *db) {
    roomdb = &db;
}

sqlite3 *get_playerdb (void) {
    return playerdb;
}

void *set_playerdb (sqlite3 *db) {
    playerdb = &db;
}
