int32_t interpret_command (const int32_t socket_num) {
    uint8_t *command = calloc(get_max_command_len(), sizeof(uint8_t));
    int32_t rv, pnum = getplayernum(socket_num);
    int32_t x, y, z;

    #define ANNOUNCE_LOGIN_TO_OTHER_PLAYERS \
        x = get_player_coord(X_COORD_REQUEST, get_player_socket(pnum));\
        y = get_player_coord(Y_COORD_REQUEST, get_player_socket(pnum));\
        z = get_player_coord(Z_COORD_REQUEST, get_player_socket(pnum));\
        strncpy(get_player_buffer(pnum), player[pnum].pname, NAMES_MAX);\
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
        if (strlen((char*)command) > get_max_command_len() || ((check_clist(socket_num, command)) != 1)) { // not a valid command, inform the player 
            print_output(socket_num, INVALCMD);
            return -1;
        }
    }

    // should probably handle 'quit' if they want to exit this process
    switch (get_player_wait_state(pnum)) {
        case THEIR_NAME:
            if (check_if_name_is_valid(socket_num, command)) break;
            if (check_if_name_is_reserved(socket_num, command)) break;
            set_player_pname(pnum, command);
            if (lookup_player(get_player_pname(pnum)) == 1) {
                print_output(socket_num, REQUEST_PW_FOR_EXISTING);
                set_player_wait_state(pnum, THEIR_PASSWORD_EXISTING);
            } else {
                print_output(socket_num, REQUEST_PW_FOR_NEW);
                set_player_wait_state(pnum, THEIR_PASSWORD_NEWPRELIM);
            }
            check_if_player_is_already_online(pnum);
            break;
        case THEIR_PASSWORD_EXISTING:
            handle_existing_pass(socket_num, command);
            break;
        case THEIR_PASSWORD_NEWPRELIM:
            set_player_confirm_new_pw(socket_num, command);
            break;
        case THEIR_PASSWORD_NEWFINAL:
            handle_new_pass(socket_num, command);
            break;
        case WAIT_ENTER_NEW_ROOM_NAME:
            init_player_store(pnum);
            print_output(socket_num, PRINT_CONFIRM_NEW_ROOM_NAME);
            break;
        case WAIT_CONFIRM_NEW_ROOM_NAME:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                rv = adjust_room_details(ADJUSTING_ROOM_NAME, -1, pnum,
                        get_player_coord(X_COORD_REQUEST, pnum),
                        get_player_coord(Y_COORD_REQUEST, pnum),
                        get_player_coord(Z_COORD_REQUEST, pnum));
                if (rv == 1) {
                    print_output(socket_num, PRINT_ADJUSTMENT_SUCCESSFUL);
                } else if (rv == -2) {
                    print_output(socket_num, PRINT_COULDNT_ADJUST_ROOM);
                } else if (rv == -3) {
                    print_output(socket_num, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
            }
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            clear_player_store(pnum);

            print_output(socket_num, SHOWROOM);
            strncpy(player[pnum].buffer, player[pnum].pname, NAMES_MAX);
            strcat(player[pnum].buffer, (char*)" changes the room name.");
            echoaround_player(pnum, ROOM_ONLY);
            break;
        case WAIT_ENTER_NEW_ROOM_DESC:
            player[pnum].store = calloc(player[pnum].buffer, sizeof(uint8_t));
            player[pnum].store_size = player[pnum].buffer;
            player[pnum].wait_state = WAIT_CONFIRM_NEW_ROOM_DESC;
            strncpy(player[pnum].store, player[pnum].buffer, BUFFER_LENGTH);
            print_output(socket_num, PRINT_CONFIRM_NEW_ROOM_DESC);
            break;
        case WAIT_CONFIRM_NEW_ROOM_DESC:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                rv = adjust_room_details(ADJUSTING_ROOM_DESC, -1, pnum,
                        get_player_coord(X_COORD_REQUEST, pnum),
                        get_player_coord(Y_COORD_REQUEST, pnum),
                        get_player_coord(Z_COORD_REQUEST, pnum));
                if (rv == 1) {
                    print_output(socket_num, PRINT_ADJUSTMENT_SUCCESSFUL);
                } else if (rv == -2) {
                    print_output(socket_num, PRINT_COULDNT_ADJUST_ROOM);
                } else if (rv == -3) {
                    print_output(socket_num, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
            }
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            clear_player_store(pnum);
            print_output(socket_num, SHOWROOM);
            break;

        case WAIT_ROOM_REMOVAL_CHECK:
            if (command[0] == 'y' || command[0] == 'Y') {
                print_output(socket_num, PRINT_ROOM_REMOVAL_CONFIRM);
                player[pnum].wait_state = WAIT_ROOM_REMOVAL_CONFIRM;
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
                player[pnum].wait_state = NO_WAIT_STATE;
                player[pnum].hold_for_input = 0;
            }
            break;
        case WAIT_ROOM_REMOVAL_CONFIRM:
            if (command[0] == 'y' || command[0] == 'Y') {
                rv = remove_room(-1, -1, -1, pnum);
                if (rv == 1) {
                    print_output(socket_num, PRINT_ROOM_REMOVAL_SUCCESS);
                } else if (rv == -2) {
                    print_output(socket_num, PRINT_ROOM_REMOVAL_FAILURE);
                } else if (rv == -3) {
                    print_output(socket_num, PRINT_INSUFFICIENT_PERMISSIONS);
                }
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
            }
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            break;
        case WAIT_ROOM_CREATION_DIR:
            if ((ensure_player_moving_valid_dir(socket_num, command)) == EXIT_FAILURE) {
                break;
            }
            player[pnum].store = calloc(strlen((char*)command) + 1, sizeof(uint8_t));
            player[pnum].store_size = strlen((char*)command) + 1; // +1 for the \0
            strncpy(player[pnum].store, (char*)command, player[pnum].store_size);
            strncat(player[pnum].store, "\0", 1);
            player[pnum].wait_state = WAIT_ROOM_CREATION_CONF;
            print_output(socket_num, PRINT_ROOM_CREATION_CONFIRMALL);
            break;
        case WAIT_ROOM_CREATION_CONF:
            if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
                x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum),
                y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum),
                z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum),
                rv = insert_room((uint8_t*)"NULL SPACE", x, y, z, 
                                 (uint8_t*)"There's nothing here but a lack of oxygen and the sense of impending doom.",
                                 player[pnum].pname, (uint8_t*)"none");
                if (rv == 1) {
                    print_output(socket_num, PRINT_ROOM_CREATION_SUCCESS);
                    rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, 
                            get_player_coord(X_COORD_REQUEST, pnum),
                            get_player_coord(Y_COORD_REQUEST, pnum),
                            get_player_coord(Z_COORD_REQUEST, pnum));
                    rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
                    print_output(socket_num, SHOWROOM);
                } else if (rv == -3) {
                    print_output(socket_num, PRINT_INSUFFICIENT_PERMISSIONS);
                } else {
                    print_output(socket_num, PRINT_ROOM_CREATION_FAILURE);
                }
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
            }
            memset(player[pnum].store, '\0', player[pnum].store_size);
            free(player[pnum].store);
            player[pnum].store = NULL;
            player[pnum].store_size = 0;
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            break;

        case WAIT_ENTER_FLAG_NAME:
            // CHECK_IF_IS_VALID_FLAG;
            // finish
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            break;
        case WAIT_ENTER_EXIT_NAME:
            if ((ensure_player_moving_valid_dir(socket_num, command)) == EXIT_FAILURE) {
                break;
            }

            player[pnum].store = calloc(strlen((char*)command), sizeof(uint8_t));
            player[pnum].store_size = strlen((char*)command);
            strncpy(player[pnum].store, (char*)command, player[pnum].store_size);

            x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum),
            y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum),
            z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum),
            rv = lookup_room(x, y, z, -1);

            if (rv == 1) {
                rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
                x = get_player_coord(X_COORD_REQUEST, pnum);
                y = get_player_coord(Y_COORD_REQUEST, pnum);
                z = get_player_coord(Z_COORD_REQUEST, pnum);
                rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, x, y, z);
                print_output(socket_num, PRINT_TOGGLED_ROOM_EXIT);
            } else if (rv == 0) {
                print_output(socket_num, PRINT_COULDNT_TOGGLE_EXIT);
            } else if (rv == -1) {
                print_output(socket_num, PRINT_COULDNT_EXIT_NO_ROOM);
            }

            memset(player[pnum].store, '\0', player[pnum].store_size);
            free(player[pnum].store);
            player[pnum].store = NULL;
            player[pnum].store_size = 0;
            player[pnum].wait_state = NO_WAIT_STATE;
            player[pnum].hold_for_input = 0;
            break;

        default:
            fprintf(stdout, "Unhandled wait state %d on player %s.\n", player[pnum].wait_state, player[pnum].pname);
            return -1;
    }
    return EXIT_SUCCESS;
}
