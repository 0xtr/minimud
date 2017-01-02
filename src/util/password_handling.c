int32_t handle_existing_pass (const int32_t socket_num, const uint8_t *command) {
    int32_t pnum = getplayernum(socket_num);
    if (get_existing_player_hash(pnum) == -1) {
        print_output(socket_num, UNABLE_TO_RETRIEVE_CHAR);
        player[pnum].wait_state = THEIR_NAME;
        memset(player_tmp.hash, '\0', HASHSPACE);
        return -1;
    }
    if (bcrypt_checkpass(command, player_tmp.hash) == -1) {
        print_output(socket_num, INCORRECT_PASSWORD);
        player[pnum].wait_state = THEIR_NAME;
    }
    if ((lookup_room(get_player_coord(X_COORD_REQUEST, pnum),
                     get_player_coord(Y_COORD_REQUEST, pnum),
                     get_player_coord(Z_COORD_REQUEST, pnum), -1)) == 0) {
        rv = adjust_player_location(pnum, -1, -1, -1);
        assert(rv == 1);
        fprintf(stdout, "[INFO] Moving player %d from a nonexistent room.\n", pnum);
    }
    print_output(socket_num, SHOWROOM);
    player[pnum].wait_state = NO_WAIT_STATE;
    player[pnum].hold_for_input = 0;
    ANNOUNCE_LOGIN_TO_OTHER_PLAYERS;
    return EXIT_SUCCESS;
}

int32_t handle_new_pass (const int32_t socket_num, const uint8_t *command) {
    int32_t pnum = getplayernum(socket_num);
    if (strcmp((char*)command, (char*)player[pnum].holder) == 0 && 
       (strlen((char*)command) == strlen((char*)player[pnum].holder))) {
        print_output(socket_num, ATTEMPT_CREATE_USR);
        if ((insert_player(player[pnum].pname, player[pnum].holder, socket_num)) == -1) {
            print_output(socket_num, PLAYER_CREATION_FAILED);
            shutdown_socket(pnum);
            return EXIT_FAILURE;
        }
        memset(player[pnum].holder, '\0', NAMES_MAX);
        player[pnum].wait_state = NO_WAIT_STATE;
        player[pnum].hold_for_input = 0;
        print_output(pnum, SHOWROOM);
        ANNOUNCE_LOGIN_TO_OTHER_PLAYERS;
        fprintf(stdout, "Player (num %d), name %s has connected from IP[%s].\n", pnum, player[pnum].pname, player[pnum].holder);
        return EXIT_SUCCESS;
    }
    print_output(socket_num, MISMATCH_PW_SET);
    player[pnum].wait_state = THEIR_NAME;
    memset(player[pnum].holder, '\0', NAMES_MAX);
    memset(player[pnum].pname, '\0', NAMES_MAX);
    return -1;
}

int32_t set_player_confirm_new_pw (const int32_t socket_num, const uint8_t *command) {
    int32_t pnum = getplayernum(socket_num);
    strncpy(player[pnum].holder, command, NAMES_MAX);
    if ((print_output(socket_num, REQUEST_PW_CONFIRM)) == 0) {
        // TODO: confirm return of print_output
        // shutdown_socket
    }
    player[pnum].wait_state = THEIR_PASSWORD_NEW;
    return EXIT_SUCCESS;
}
