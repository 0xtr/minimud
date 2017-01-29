int32_t handle_existing_pass (const int32_t pnum, const uint8_t *command) {
    if (get_existing_player_hash(pnum) == -1) {
        print_output(pnum, UNABLE_TO_RETRIEVE_CHAR);
        set_player_wait_state(pnum, THEIR_NAME);
        return EXIT_FAILURE;
    }
    if (bcrypt_checkpass(command, get_existing_player_hash(pnum)) == -1) {
        print_output(pnum, INCORRECT_PASSWORD);
        set_player_wait_state(pnum, THEIR_NAME);
        return EXIT_FAILURE;
    }
    if (lookup_room(get_player_coord(X_COORD_REQUEST, pnum),
                     get_player_coord(Y_COORD_REQUEST, pnum),
                     get_player_coord(Z_COORD_REQUEST, pnum), -1) == 0) {
        assert(adjust_player_location(pnum, -1, -1, -1) == EXIT_SUCCESS);
        fprintf(stdout, "[INFO] Moving player %d from a nonexistent room.\n", pnum);
    }
    print_output(pnum, SHOWROOM);
    set_player_wait_state(pnum, NO_WAIT_STATE);
    set_player_hold_for_input(pnum, 0);
    return EXIT_SUCCESS;
}

int32_t handle_new_pass (const int32_t pnum, const uint8_t *command) {
    if (strcmp((char*)command, (char*)get_player_store(pnum)) != 0 && 
       (strlen((char*)command) != strlen((char*)get_player_store(pnum)))) {
        print_output(pnum, MISMATCH_PW_SET);
        set_player_wait_state(pnum, THEIR_NAME);
        return EXIT_FAILURE;
    }
    print_output(pnum, ATTEMPT_CREATE_USR);
    if ((insert_player(get_player_pname(pnum), get_player_store(pnum), pnum)) == -1) {
        print_output(pnum, PLAYER_CREATION_FAILED);
        shutdown_socket(pnum);
        return EXIT_FAILURE;
    }
    set_player_wait_state(pnum, NO_WAIT_STATE);
    set_player_hold_for_input(pnum, 0);
    print_output(pnum, SHOWROOM);
    fprintf(stdout, "Player (num %d), name %s has connected.\n", pnum, get_player_pname(pnum));
    return EXIT_SUCCESS;
}

int32_t set_player_confirm_new_pw (const int32_t pnum, const uint8_t *command) {
    set_player_store_replace(pnum, command);
    if ((print_output(pnum, REQUEST_PW_CONFIRM)) == 0) {
        // TODO: confirm return of print_output
        // shutdown_socket
    }
    set_player_wait_state(pnum, THEIR_PASSWORD_NEWFINAL);
    return EXIT_SUCCESS;
}
