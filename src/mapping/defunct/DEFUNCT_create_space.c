int create_space (void) {
    #define  MIN_DIRS                 0+1
    #define  MAX_DIRS                 3+1
    #define  MAX_DICEROLLS            100
    #define  DICEROLL_ENSURE_NO_REPEAT \
        for (iters = 0; iters != MAX_DICEROLLS; ++iters) {\
            diceroll = random_range(MIN_DIRS,MAX_DIRS);\
            if (diceroll == 0) {\
                continue;\
            } else {\
                break;\
            }\
        }
    #define ENSURE_NO_COLLISION \
        if ((MAP_WIDTH / 5)  >= START_LOC_X) {\
            if (diceroll == 4) {\
                DICEROLL_ENSURE_NO_REPEAT;\
            }\
        }\
        if ((MAP_HEIGHT / 5) >= START_LOC_Y) {\
            if (diceroll == 2) {\
                DICEROLL_ENSURE_NO_REPEAT;\
            }\
        }\
        if ((START_LOC_X >= ((MAP_WIDTH / 5) * 4))) {\
            if (diceroll == 3) {\
                DICEROLL_ENSURE_NO_REPEAT;\
            }\
        }\
        if ((START_LOC_Y >= ((MAP_HEIGHT / 5) * 4))) {\
            if (diceroll == 1) {\
                DICEROLL_ENSURE_NO_REPEAT;\
            }\
        }

    #define CHECK_THEN_REROLL \
        ++simultaneous_rerolls, --i;\
        if (simultaneous_rerolls == MAX_DICEROLLS) {\
            simultaneous_rerolls = 0;\
            diceroll = -1;\
            continue;\
        }\
        continue;
    int   i, diceroll = 0;
    int   CURR_X      = START_LOC_X;
    int   CURR_Y      = START_LOC_Y;
    int   CURR_Z      = START_LOC_Z;
    int   iters, simultaneous_rerolls = 0;
    map[CURR_X][CURR_Y][CURR_Y].is_occupied = 1;

    // Insert into SQLITE db
    sqlite_insert_room(BASE_ROOM_NAME, 
                       CURR_X, CURR_Y, CURR_Z,
                       BASE_ROOM_DESC,
                       "0","0","0","0","0","0","0","0","0","0");

    for (i = 0; i != MAX_ROOMS; ++i) {
        if (diceroll == -1) {
            break;
        }
        DICEROLL_ENSURE_NO_REPEAT;
        ENSURE_NO_COLLISION;
        switch (diceroll) {
            case 1: // north
                if (map[CURR_X][CURR_Y+1][CURR_Z].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_Y+1 > MAP_HEIGHT) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].north = 1;
                ++CURR_Y;
                map[CURR_X][CURR_Y][CURR_Z].south = 1;
                break;
            case 2: // south
                if (map[CURR_X][CURR_Y-1][CURR_Z].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_Y-1 < MAP_HEIGHT) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].south = 1;
                --CURR_Y;
                map[CURR_X][CURR_Y][CURR_Z].north = 1;
                break;
            case 3: // east
                if (map[CURR_X+1][CURR_Y][CURR_Z].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_X+1 > MAP_WIDTH) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].east  = 1;
                ++CURR_X;
                map[CURR_X][CURR_Y][CURR_Z].west  = 1;
                break;
            case 4: // west
                if (map[CURR_X-1][CURR_Y][CURR_Z].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_X-1 < MAP_WIDTH) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].west  = 1;
                --CURR_X;
                map[CURR_X][CURR_Y][CURR_Z].east  = 1;
                break;
            case 5: // down
                if (map[CURR_X][CURR_Y][CURR_Z-1].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_Z-1 < MAP_DEPTH) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].down  = 1;
                --CURR_Z;
                map[CURR_X][CURR_Y][CURR_Z].up  = 1;
                break;
            case 6: // up
                if (map[CURR_X][CURR_Y][CURR_Z+1].is_occupied == 1 ||
                    CURR_X < 0 || CURR_Y < 0 || CURR_Z < 0 || CURR_Z+1 > MAP_DEPTH) {
                    CHECK_THEN_REROLL;
                }
                map[CURR_X][CURR_Y][CURR_Z].up  = 1;
                ++CURR_Z;
                map[CURR_X][CURR_Y][CURR_Z].down  = 1;
                break;
            default:
                log_issue("Logic failure during space creation.");
                exit(EXIT_FAILURE);
        }
        simultaneous_rerolls      = 0;
        printf("occu pre: %d\n", map[CURR_X][CURR_Y][CURR_Y].is_occupied); 
        printf("%d %d %d\n", CURR_X, CURR_Y, CURR_Z);
        map[CURR_X][CURR_Y][CURR_Y].is_occupied = 1;
        printf("occu post: %d\n\n", map[CURR_X][CURR_Y][CURR_Y].is_occupied); 
    }

    assert(i <= MAX_ROOMS);
    if (i <= MAX_ROOMS) {
        // reroll if incomplete
        if (diceroll == -1 && i != 1) {
            return i;
        }
        #ifdef MAP_PRINT_COMPLETE_OUTPUT
            // print a basic map of the structure
            int x, y;
            for (y = MAP_HEIGHT; y != 0; --y) {
                for (x = 0; x != MAP_WIDTH; ++x) {
                    if (map[x][y].is_occupied == 1) {
                        if (x == START_LOC_X && y == START_LOC_Y && z == START_LOC_Z) {
                            fprintf(stdout, "@");
                        } else {
                            fprintf(stdout, "-");
                        }
                    } else {
                        fprintf(stdout, "0");
                    }
                }
                if (x == MAP_WIDTH) {
                    fprintf(stdout, "\n");
                }
            }
        #endif
        fprintf(stdout, "%s\n", "Path generation complete.");
        return 1;
    }
    return 0;
}
