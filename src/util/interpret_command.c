int32_t interpret_command (const int socket_num) {
    uint8_t command[MAX_COMMAND_LEN] = {0};
    int32_t rv, pnum = getplayernum(socket_num);
    int32_t x, y, z;

    #define ANNOUNCE_LOGIN_TO_OTHER_PLAYERS \
        x = get_player_coord(X_COORD_REQUEST, player[pnum].socket_num);\
        y = get_player_coord(Y_COORD_REQUEST, player[pnum].socket_num);\
        z = get_player_coord(Z_COORD_REQUEST, player[pnum].socket_num);\
        strncpy(player[pnum].buffer, player[pnum].pname, NAMES_MAX);\
        strcat((char*)player[pnum].buffer, " has connected, in location [");\
        rv = lookup_room_name_from_coords(x, y, z);\
        strcat(player[pnum].buffer, "].");\
        echoaround_player(pnum, ALL_PLAYERS);\

    // -------------------------------------------------------

    if (player[pnum].wait_state != THEIR_NAME) {
        for (size_t i = 0; i < (MAX_COMMAND_LEN * 2); ++i) {
            command[i] = player[pnum].buffer[i];
            if (player[pnum].buffer[i] == ' ' || player[pnum].buffer[i] == '\n' || player[pnum].buffer[i] == '\0' || player[pnum].buffer[i] == '\r') {
                command[i] = '\0';
                break;
            }
        }
    }
    if (player[pnum].hold_for_input == 1 && strcmp((char*)command, "quit") != 0) {
        if (strlen(command) > MAX_COMMAND_LEN || ((check_clist(socket_num, command)) != 1)) { // not a valid command, inform the player 
            print_output(socket_num, INVALCMD);
            return -1;
        }
    }

    // should probably handle 'quit' if they want to exit this process
    switch (player[pnum].wait_state) {
        case THEIR_NAME:
            if ((check_if_name_is_valid(socket_num, command)) == 1) break;
            if ((check_if_name_is_reserved(socket_num, command)) == 1) break;
            strncpy((char*)player[pnum].pname, (char*)command, NAMES_MAX);
            if (lookup_player(player[pnum].pname) == 1) {
                print_output(socket_num, REQUEST_PW_FOR_EXISTING);
                player[pnum].wait_state = THEIR_PASSWORD_EXISTING;
            } else {
                print_output(socket_num, REQUEST_PW_FOR_NEW);
                player[pnum].wait_state = THEIR_PASSWORD_NEWPRELIM;
            }
            for (size_t i = 0; i < get_active_conns(); ++i) {
                if (strcmp(player[pnum].pname, player[i].pname) == 0 && i != pnum) {
                    print_output(socket_num, PLAYER_ALREADY_ONLINE);
                    player[pnum].wait_state = THEIR_NAME;
                }
            }
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
            player[pnum].store = malloc(NAMES_MAX);
            player[pnum].store_size = NAMES_MAX;
            player[pnum].wait_state = WAIT_CONFIRM_NEW_ROOM_NAME;
            memset(player[pnum].store, '\0', NAMES_MAX);
            strncpy(player[pnum].store, player[pnum].buffer, NAMES_MAX);
            print_output(socket_num, PRINT_CONFIRM_NEW_ROOM_NAME);
            break;
        case WAIT_CONFIRM_NEW_ROOM_NAME:
            if ((strcmp(command, "y") == 0) || (strcmp(command, "Y") == 0)) {
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

            memset(player[pnum].store, '\0', player[pnum].store_size);
            free(player[pnum].store);
            player[pnum].store = NULL;
            player[pnum].store_size = 0;

            print_output(socket_num, SHOWROOM);
            strncpy(player[pnum].buffer, player[pnum].pname, NAMES_MAX);
            strcat(player[pnum].buffer, (char*)" changes the room name.");
            echoaround_player(pnum, ROOM_ONLY);
            break;
        case WAIT_ENTER_NEW_ROOM_DESC:
            player[pnum].store = malloc(player[pnum].bufferLEN);
            player[pnum].store_size = player[pnum].bufferLEN;
            player[pnum].wait_state = WAIT_CONFIRM_NEW_ROOM_DESC;
            strncpy(player[pnum].store, player[pnum].buffer, BUFLEN);
            print_output(socket_num, PRINT_CONFIRM_NEW_ROOM_DESC);
            break;
        case WAIT_CONFIRM_NEW_ROOM_DESC:
            if ((strcmp(command, "y") == 0) || (strcmp(command, "Y") == 0)) {
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

            memset(player[pnum].store, '\0', player[pnum].store_size);
            free(player[pnum].store);
            player[pnum].store = NULL;
            player[pnum].store_size = 0;

            print_output(socket_num, SHOWROOM);
            strncpy(player[pnum].buffer, player[pnum].pname, NAMES_MAX);
            strcat(player[pnum].buffer, (char*)"changes the room description.");
            echoaround_player(pnum, ROOM_ONLY);
            break;

        case WAIT_ROOM_REMOVAL_CHECK:
            if ((strcmp(command, "y") == 0) || (strcmp(command, "Y") == 0)) {
                print_output(socket_num, PRINT_ROOM_REMOVAL_CONFIRM);
                player[pnum].wait_state = WAIT_ROOM_REMOVAL_CONFIRM;
            } else {
                print_output(socket_num, PRINT_EXITING_CMD_WAIT);
                player[pnum].wait_state = NO_WAIT_STATE;
                player[pnum].hold_for_input = 0;
            }
            break;
        case WAIT_ROOM_REMOVAL_CONFIRM:
            if ((strcmp(command, "y") == 0) || (strcmp(command, "Y") == 0)) {
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
            player[pnum].store = malloc(strlen(command) + 1);
            player[pnum].store_size = strlen(command) + 1; // +1 for the \0
            strncpy(player[pnum].store, command, player[pnum].store_size);
            strncat(player[pnum].store, "\0", 1);
            player[pnum].wait_state = WAIT_ROOM_CREATION_CONF;
            print_output(socket_num, PRINT_ROOM_CREATION_CONFIRMALL);
            break;
        case WAIT_ROOM_CREATION_CONF:
            if ((strcmp(command, "y") == 0) || (strcmp(command, "Y") == 0)) {
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

            player[pnum].store = malloc(strlen(command));
            player[pnum].store_size = strlen(command);
            memset(player[pnum].store, '\0', player[pnum].store_size);
            strncpy(player[pnum].store, command, player[pnum].store_size);

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
