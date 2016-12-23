int32_t move_player (int32_t socknum, const uint8_t command[]) {
    int32_t rv = 0;
    int32_t movedir;
    int32_t num = getplayernum(socknum);
    int32_t x = get_player_coord(X_COORD_REQUEST, num);
    int32_t y = get_player_coord(Y_COORD_REQUEST, num);
    int32_t z = get_player_coord(Z_COORD_REQUEST, num);
    int32_t xadj, yadj, zadjt;
    xadj = yadj = zadjt = 0;

    // new funcs here
    if (strcmp((char*)command, "north") == 0 || strcmp((char*)command, "n") == 0) {
        movedir = 1;
        y = y + 1;
        yadj = 1;
    } else if (strcmp((char*)command, "east")  == 0 || strcmp((char*)command, "e") == 0) {
        movedir = 2;
        x = x + 1;
        xadj = 1;
    } else if (strcmp((char*)command, "south") == 0 || strcmp((char*)command, "s") == 0) {
        movedir = 3;
        y = y - 1;
        yadj = -1;
    } else if (strcmp((char*)command, "west")  == 0 || strcmp((char*)command, "w") == 0) {
        movedir = 4;
        x = x - 1;
        xadj = -1;
    } else if (strcmp((char*)command, "down")  == 0 || strcmp((char*)command, "d") == 0) {
        movedir = 5;
        z = z - 1;
        zadj = -1;
    } else if (strcmp((char*)command, "up")    == 0 || strcmp((char*)command, "u") == 0) {
        movedir = 6;
        z = z + 1;
        zadj = 1;
    } else if (strcmp((char*)command, "northwest") == 0 || strcmp((char*)command, "nw") == 0) {
        movedir = 7;
        x = x - 1;
        y = y + 1;
        xadj = -1;
        yadj = 1;
    } else if (strcmp((char*)command, "northeast") == 0 || strcmp((char*)command, "ne") == 0) {
        movedir = 8;
        x = x + 1;
        y = y + 1;
        xadj = 1;
        yadj = 1;
    } else if (strcmp((char*)command, "southwest") == 0 || strcmp((char*)command, "sw") == 0) {
        movedir = 9;
        y = y - 1;
        x = x - 1;
        xadj = -1;
        yadj = -1;
    } else if (strcmp((char*)command, "southeast") == 0 || strcmp((char*)command, "se") == 0) {
        movedir = 10;
        x = x + 1;
        y = y - 1;
        xadj = 1;
        yadj = -1;
    } else if (strcmp((char*)command, "return") == 0) {
        movedir = 0; // back to origin room
        x = -1;
        y = -1;
        z = -1;
    }
    if (movedir != 0) {
        rv = lookup_room_exits(num, xadj, yadj, zadj);
    }
    if (rv == -1) {
        rv = print_output(num, INVALDIR);
        return -1;
    } else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
        rv = print_output(num, INVALDIR);
        x = -1;
        y = -1;
        z = -1;
        movedir = 0;
    }
    rv = print_output(num, movedir);
    if (rv == 2) {
        rv = adjust_player_location(num, x, y, z);
        return 1;
    } else if (rv == 0) {
        rv = adjust_player_location(num, -1, -1, -1);
        return 1;
    } else {
        return -1;
    }
}
