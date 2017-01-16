int32_t echoaround_player (const int32_t pnum, const int32_t override) {
    int32_t status = 0;
    int32_t X = get_player_coord(X_COORD_REQUEST, pnum);
    int32_t Y = get_player_coord(Y_COORD_REQUEST, pnum);
    int32_t Z = get_player_coord(Z_COORD_REQUEST, pnum);
    size_t j = 0;
    for (size_t i = 0; i < get_active_conns(); ++i, ++j) {
        if (j != pnum) {
            if (get_player_socket(j) != get_player_socket(pnum)) {
                if (get_player_in_use(j) == 1 && get_player_in_use(pnum) == 1) {
                    if (get_player_coord(X_COORD_REQUEST, j) == X &&
                        get_player_coord(Y_COORD_REQUEST, j) == Y &&
                        get_player_coord(Z_COORD_REQUEST, j) == Z) {
                        if (send_and_ensure(j, NULL) != EXIT_SUCCESS) {
                            log_issue((uint8_t*)"Problem sending say data.");
                            break;
                        }
                    }
                    if (override == ALL_PLAYERS) {
                        if (send_and_ensure(j, NULL) != EXIT_SUCCESS) {
                            log_issue((uint8_t*)"Problem sending say data.");
                            break;
                        }
                    }
                }
            }
        }
    }
    if (status != 1) {
        return 0;
    }
    return 1;
}
