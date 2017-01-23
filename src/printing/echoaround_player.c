int32_t echoaround_player (const int32_t pnum, const uint8_t *buffer, const int32_t location) {
    int32_t X = get_player_coord(X_COORD_REQUEST, pnum);
    int32_t Y = get_player_coord(Y_COORD_REQUEST, pnum);
    int32_t Z = get_player_coord(Z_COORD_REQUEST, pnum);
    int32_t j = 0;
    for (size_t i = 0; i < get_num_of_players(); ++i, ++j) {
        if (get_player_socket(j) != get_player_socket(pnum)) {
            if (location == ROOM_ONLY) {
                if (get_player_coord(X_COORD_REQUEST, j) == X &&
                    get_player_coord(Y_COORD_REQUEST, j) == Y &&
                    get_player_coord(Z_COORD_REQUEST, j) == Z) {
                    set_player_buffer_replace(j, buffer);
                    assert(outgoing_msg_handler(j) == EXIT_SUCCESS);
                }
            } else if (location == ALL_PLAYERS) {
                set_player_buffer_replace(j, buffer);
                assert(outgoing_msg_handler(j) == EXIT_SUCCESS);
            }
        }
    }
    return EXIT_SUCCESS;
}
