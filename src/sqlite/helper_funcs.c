int callback (void *map, int argc, char **argv, char **azColName) {
    Map *map_ref = map;
    increment_sqlite_rows_count();
    for (size_t i = 0; i < (size_t)argc; ++i) {

        if (map != NULL) {
            if (strcmp(azColName[i], "rname") == 0) {
                strncpy((char*)map_ref->rname, argv[i], NAMES_MAX);
            } else if (strcmp(azColName[i], "rdesc") == 0) {
                strncpy((char*)map_ref->rdesc, argv[i], BUFFER_LENGTH);
            } else if (strcmp(azColName[i], "north") == 0) {
                map_ref->north = atoi(argv[i]);
            } else if (strcmp(azColName[i], "east") == 0) {
                map_ref->east = atoi(argv[i]);
            } else if (strcmp(azColName[i], "south") == 0) {
                map_ref->south = atoi(argv[i]);
            } else if (strcmp(azColName[i], "west") == 0) {
                map_ref->west = atoi(argv[i]);
            } else if (strcmp(azColName[i], "up") == 0) {
                map_ref->up = atoi(argv[i]);
            } else if (strcmp(azColName[i], "down") == 0) {
                map_ref->down = atoi(argv[i]);
            } else if (strcmp(azColName[i], "northeast") == 0) {
                map_ref->northeast = atoi(argv[i]);
            } else if (strcmp(azColName[i], "southeast") == 0) {
                map_ref->southeast = atoi(argv[i]);
            } else if (strcmp(azColName[i], "southwest") == 0) {
                map_ref->southwest = atoi(argv[i]);
            } else if (strcmp(azColName[i], "northwest") == 0) {
                map_ref->northwest = atoi(argv[i]);
            } else if (strcmp(azColName[i], "owner") == 0) {
                strncpy((char*)map_ref->owner, argv[i], NAMES_MAX);
            }
        } else { // players
            if (azColName[i][0] == 'x') {
                //player_tmp.x = atoi(argv[i]);
            } else if (azColName[i][0], "y") {
                //player_tmp.y = atoi(argv[i]);
            } else if (azColName[i][0] == 'z') {
                //player_tmp.z = atoi(argv[i]);
            } else if (strcmp(azColName[i], "hash") == 0) {
                //strncpy(player_tmp.hash, (char*)argv[i], HASHSPACE);
            }
        }

    }
    // return 0 or callback will request an abort
    return EXIT_SUCCESS;
}

static int rows_count;
int32_t get_sqlite_rows_count () {
    return rows_count;
}
void increment_sqlite_rows_count () {
    ++rows_count;
}
void set_sqlite_rows_count (const int32_t newval) {
    rows_count = newval;
}
void reset_sqlite_rows_count () {
    rows_count = 0;
}
