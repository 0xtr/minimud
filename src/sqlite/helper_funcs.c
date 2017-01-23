int callback (void *NotUsed __attribute__ ((unused)), int argc, char **argv, char **azColName __attribute__ ((unused))) {
    increment_sqlite_rows_count();
    for (size_t i = 0; i < (size_t)argc; ++i) {
        if (strcmp(azColName[i], "rname") == 0) {
            if (argv[i] != NULL) {
                strncpy(map.rname, argv[i], NAMES_MAX);
            }
        } else if (strcmp(azColName[i], "rdesc") == 0) {
            if (argv[i] != NULL) {
                strncpy(map.rdesc, argv[i], BUFFER_LENGTH);
            }
        } else if (strcmp(azColName[i], "north") == 0) {
            if (argv[i] != NULL) {
                map.north = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "east") == 0) {
            if (argv[i] != NULL) {
                map.east = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "south") == 0) {
            if (argv[i] != NULL) {
                map.south = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "west") == 0) {
            if (argv[i] != NULL) {
                map.west = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "up") == 0) {
            if (argv[i] != NULL) {
                map.up = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "down") == 0) {
            if (argv[i] != NULL) {
                map.down = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "northeast") == 0) {
            if (argv[i] != NULL) {
                map.northeast = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "southeast") == 0) {
            if (argv[i] != NULL) {
                map.southeast = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "southwest") == 0) {
            if (argv[i] != NULL) {
                map.southwest = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "northwest") == 0) {
            if (argv[i] != NULL) {
                map.northwest = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "owner") == 0) {
            if (argv[i] != NULL) {
                strncpy(map.owner, argv[i], NAMES_MAX);
            }
        // players below
        //} else if (strcmp(azColName[i], "x") == 0) {
        } else if (azColName[i] == 'x') {
            if (argv[i] != NULL) {
                player_tmp.x = atoi(argv[i]);
            }
        //} else if (strcmp(azColName[i], "y") == 0) {
        } else if (azColName[i], "y") {
            if (argv[i] != NULL) {
                player_tmp.y = atoi(argv[i]);
            }
        //} else if (strcmp(azColName[i], "z") == 0) {
        } else if (azColName[i] == 'z') {
            if (argv[i] != NULL) {
                player_tmp.z = atoi(argv[i]);
            }
        } else if (strcmp(azColName[i], "hash") == 0) {
            if (argv[i] != NULL) {
                strncpy(player_tmp.hash, (char*)argv[i], HASHSPACE);
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
