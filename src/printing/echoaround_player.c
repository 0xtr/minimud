int32_t echoaround_player (int32_t pnum, int32_t override) {
    int32_t j, status = 0;
    int32_t X = get_player_coord(X_COORD_REQUEST, pnum);
    int32_t Y = get_player_coord(Y_COORD_REQUEST, pnum);
    int32_t Z = get_player_coord(Z_COORD_REQUEST, pnum);
    for (size_t i = 0, j = 0; i <= MAX_CONNS; ++i, ++j) {
        if (j != pnum) {
            if (player[j].socknum != player[pnum].socknum) {
                if (player[j].in_use == 1 && player[pnum].in_use == 1) {
                    if (get_player_coord(X_COORD_REQUEST, j) == X &&
                        get_player_coord(Y_COORD_REQUEST, j) == Y &&
                        get_player_coord(Z_COORD_REQUEST, j) == Z) {
                        wfd = player[j].socknum;
                        status = send_and_ensure(j);
                        if (status != 1) {
                            log_issue("Problem sending say data.");
                            break;
                        }
                    }
                    if (override == ALL_PLAYERS) {
                        wfd = player[j].socknum;
                        status = send_and_ensure(j);
                        if (status != 1) {
                            log_issue("Problem sending say data.");
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
