int32_t interpret_command (const int32_t pnum) {
    uint8_t *command = calloc(get_max_command_len(), sizeof(uint8_t));
    int32_t rv;
    int32_t x, y, z;

    #define ANNOUNCE_LOGIN_TO_OTHER_PLAYERS \
        x = get_player_coord(X_COORD_REQUEST, get_player_socket(pnum));\
        y = get_player_coord(Y_COORD_REQUEST, get_player_socket(pnum));\
        z = get_player_coord(Z_COORD_REQUEST, get_player_socket(pnum));\
        strncpy(get_player_buffer(pnum), get_player_pname(pnum), NAMES_MAX);\
        strcat((char*)get_player_buffer(pnum), " has connected, in location [");\
        rv = lookup_room_name_from_coords(x, y, z);\
        strcat(get_player_buffer(pnum), "].");\
        echoaround_player(pnum, ALL_PLAYERS);\

    // -------------------------------------------------------

    if (get_player_wait_state(pnum) != THEIR_NAME) {
        for (size_t i = 0; i < (get_max_command_len() * 2); ++i) {
            command[i] = get_player_buffer(pnum)[i];
            if (isspace(get_player_buffer(pnum)[i]) != 0) {
                command[i] = '\0';
                break;
            }
        }
    }
    if (get_player_hold_for_input(pnum) == 1 && strcmp((char*)command, "quit") != 0) {
        if (strlen((char*)command) > get_max_command_len() || 
            ((check_clist(pnum, command)) != 1)) { // not a valid command, inform the player 
            print_output(pnum, INVALCMD);
            return -1;
        }
    }

    // should probably handle 'quit' if they want to exit this process
    switch (get_player_wait_state(pnum)) {
        case THEIR_NAME:
            if (check_if_name_is_valid(pnum, command)) break;
            if (check_if_name_is_reserved(pnum, command)) break;
            set_player_pname(pnum, command);
            if (lookup_player(get_player_pname(pnum)) == 1) {
                print_output(pnum, REQUEST_PW_FOR_EXISTING);
                set_player_wait_state(pnum, THEIR_PASSWORD_EXISTING);
            } else {
                print_output(pnum, REQUEST_PW_FOR_NEW);
                set_player_wait_state(pnum, THEIR_PASSWORD_NEWPRELIM);
            }
            check_if_player_is_already_online(pnum);
            break;
        case THEIR_PASSWORD_EXISTING:
            handle_existing_pass(pnum, command);
            break;
        case THEIR_PASSWORD_NEWPRELIM:
            set_player_confirm_new_pw(pnum, command);
            break;
        case THEIR_PASSWORD_NEWFINAL:
            handle_new_pass(pnum, command);
            break;
        case WAIT_ENTER_NEW_ROOM_NAME:
            init_player_store(pnum);
            set_player_wait_state (pnum, WAIT_CONFIRM_NEW_ROOM_NAME);
            print_output(pnum, PRINT_CONFIRM_NEW_ROOM_NAME);
            break;
        case WAIT_CONFIRM_NEW_ROOM_NAME:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                rv = adjust_room_details(ADJUSTING_ROOM_NAME, -1, pnum,
                        get_player_coord(X_COORD_REQUEST, pnum),
                        get_player_coord(Y_COORD_REQUEST, pnum),
                        get_player_coord(Z_COORD_REQUEST, pnum));
                if (rv == 1) {
                    print_output(pnum, PRINT_ADJUSTMENT_SUCCESSFUL);
                } else if (rv == -2) {
                    print_output(pnum, PRINT_COULDNT_ADJUST_ROOM);
                } else if (rv == -3) {
                    print_output(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(pnum, PRINT_EXITING_CMD_WAIT);
            }
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            clear_player_store(pnum);

            print_output(pnum, SHOWROOM);
            set_player_buffer_replace(pnum, get_player_pname(pnum));
            set_player_buffer_append(pnum, (uint8_t*)" changes the room name.");
            echoaround_player(pnum, ROOM_ONLY);
            break;
        case WAIT_ENTER_NEW_ROOM_DESC:
            init_player_store(pnum);
            set_player_wait_state(pnum, WAIT_CONFIRM_NEW_ROOM_DESC);
            set_player_store_replace(pnum, get_player_buffer(pnum));
            print_output(pnum, PRINT_CONFIRM_NEW_ROOM_DESC);
            break;
        case WAIT_CONFIRM_NEW_ROOM_DESC:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                rv = adjust_room_details(ADJUSTING_ROOM_DESC, -1, pnum,
                        get_player_coord(X_COORD_REQUEST, pnum),
                        get_player_coord(Y_COORD_REQUEST, pnum),
                        get_player_coord(Z_COORD_REQUEST, pnum));
                if (rv == 1) {
                    print_output(pnum, PRINT_ADJUSTMENT_SUCCESSFUL);
                } else if (rv == -2) {
                    print_output(pnum, PRINT_COULDNT_ADJUST_ROOM);
                } else if (rv == -3) {
                    print_output(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(pnum, PRINT_EXITING_CMD_WAIT);
            }
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            clear_player_store(pnum);
            print_output(pnum, SHOWROOM);
            break;

        case WAIT_ROOM_REMOVAL_CHECK:
            if (command[0] == 'y' || command[0] == 'Y') {
                print_output(pnum, PRINT_ROOM_REMOVAL_CONFIRM);
                set_player_wait_state(pnum, WAIT_ROOM_REMOVAL_CONFIRM);
            } else {
                print_output(pnum, PRINT_EXITING_CMD_WAIT);
                set_player_wait_state(pnum, NO_WAIT_STATE);
                set_player_hold_for_input(pnum, 0);
            }
            break;
        case WAIT_ROOM_REMOVAL_CONFIRM:
            if (command[0] == 'y' || command[0] == 'Y') {
                rv = remove_room(-1, -1, -1, pnum);
                if (rv == 1) {
                    print_output(pnum, PRINT_ROOM_REMOVAL_SUCCESS);
                } else if (rv == -2) {
                    print_output(pnum, PRINT_ROOM_REMOVAL_FAILURE);
                } else if (rv == -3) {
                    print_output(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(pnum, PRINT_EXITING_CMD_WAIT);
            }
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            break;
        case WAIT_ROOM_CREATION_DIR:
            if ((ensure_player_moving_valid_dir(pnum, command)) == EXIT_FAILURE) {
                break;
            }
            init_player_store(pnum);
            set_player_store_replace(pnum, command);
            set_player_store_append(pnum, (uint8_t*)"\0");
            set_player_wait_state(pnum, WAIT_ROOM_CREATION_CONF);
            print_output(pnum, PRINT_ROOM_CREATION_CONFIRMALL);
            break;
        case WAIT_ROOM_CREATION_CONF:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum),
                y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum),
                z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum),
                rv = insert_room((uint8_t*)"NULL SPACE", x, y, z, 
                                 (uint8_t*)"There's nothing here but a lack of oxygen and the sense of impending doom.",
                                 get_player_pname(pnum), (uint8_t*)"none");
                if (rv == 1) {
                    print_output(pnum, PRINT_ROOM_CREATION_SUCCESS);
                    rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, 
                            get_player_coord(X_COORD_REQUEST, pnum),
                            get_player_coord(Y_COORD_REQUEST, pnum),
                            get_player_coord(Z_COORD_REQUEST, pnum));
                    rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
                    print_output(pnum, SHOWROOM);
                } else if (rv == -3) {
                    print_output(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
                } else {
                    print_output(pnum, PRINT_ROOM_CREATION_FAILURE);
                }
            } else {
                print_output(pnum, PRINT_EXITING_CMD_WAIT);
            }
            clear_player_store(pnum);
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            break;

        case WAIT_ENTER_FLAG_NAME:
            // CHECK_IF_IS_VALID_FLAG;
            // finish
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            break;
        case WAIT_ENTER_EXIT_NAME:
            if ((ensure_player_moving_valid_dir(pnum, command)) == EXIT_FAILURE) {
                break;
            }

            init_player_store(pnum);
            set_player_store_replace(pnum, command);
            x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum);
            y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum);
            z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum);
            Map *map = lookup_room(x, y, z, -1);

            if (rv == 1) {
                rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
                x = get_player_coord(X_COORD_REQUEST, pnum);
                y = get_player_coord(Y_COORD_REQUEST, pnum);
                z = get_player_coord(Z_COORD_REQUEST, pnum);
                rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, x, y, z);
                print_output(pnum, PRINT_TOGGLED_ROOM_EXIT);
            } else if (rv == 0) {
                print_output(pnum, PRINT_COULDNT_TOGGLE_EXIT);
            } else if (rv == -1) {
                print_output(pnum, PRINT_COULDNT_EXIT_NO_ROOM);
            }

            free(map);
            clear_player_store(pnum);
            set_player_wait_state(pnum, NO_WAIT_STATE);
            set_player_hold_for_input(pnum, 0);
            break;

        default:
            fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(pnum), get_player_pname(pnum));
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
