int32_t remove_players_from_room (int32_t x, int32_t y, int32_t z) {
    for (size_t i = 0; i != MAX_CONNS && i < conns; ++i) {
        if (player[i].in_use == 1) {
            if (get_player_coord(X_COORD_REQUEST, i) == x &&
                get_player_coord(Y_COORD_REQUEST, i) == y &&
                get_player_coord(Z_COORD_REQUEST, i) == z) {
                print_output(player[i].socknum, PRINT_REMOVED_FROM_ROOM);
                adjust_player_location(i, -1, -1, -1);
                print_output(player[i].socknum, SHOWROOM);
            }
        }
    }
    return 0;
}
