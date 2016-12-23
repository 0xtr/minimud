int32_t print_output (int32_t socknum, int32_t argument) {
    clist   *tmp1 = head;
    int32_t num   = getplayernum(socknum);
    int32_t rv    = 0;
    int32_t room_x = get_player_coord(X_COORD_REQUEST, num);
    int32_t room_y = get_player_coord(Y_COORD_REQUEST, num);
    int32_t room_z = get_player_coord(Z_COORD_REQUEST, num);

    int32_t dont_send_again = 0;
    size_t  len = 0;
    uint8_t PWLEN_TMP[3];
    uint8_t names_max[sizeof(NAMES_MAX)];
    #define DIRS     (argument >= 0 && argument <= 10)

    CLEAR_BUFFER;
    switch (argument) {
        case PROMPT: 
            strcpy((char*)buf, "> ");
            break;
        case INVALCMD:
            strcpy((char*)buf, "Invalid command. Type \'commands\'.\n");
            break;
        case SHOWCMDS:
            strncpy((char*)buf, "> Available commands:", BUFLEN);
            rv = send_and_ensure(socknum);
            len = 0;
            for (size_t i = 0; i != SUPPORTED_COMMANDS; ++i) {
                len += strlen(tmp1->cname); 
            }
            int32_t c = 1;
            if (len > BUFLEN) {
                c = len % BUFLEN;
                if (c == 1) {
                    ++c;
                }
                printf("c buffers required: %d\n", c);
            }
            int32_t trip_first       = 0;
            int32_t commands_on_line = 0;
            for (; c != 0; c--) {
                while (tmp1) {
                    if (1 + strlen(buf) + strlen(tmp1->cname) <= BUFLEN) {
                        if (strlen(tmp1->cname) > 1) {
                            if (trip_first == 0) {
                                strncpy((char*)buf, tmp1->cname, strlen(tmp1->cname)); 
                                trip_first = 1;
                            } else {
                                strncat((char*)buf, tmp1->cname, strlen(tmp1->cname)); 
                            }
                            if (++commands_on_line != 5) {
                                strncat((char*)buf, "\t", 1);
                                if (strlen(tmp1->cname) < 5) {
                                    strncat((char*)buf, "\t", 1);
                                }
                            }
                        }
                        tmp1 = tmp1->next;
                    } else {
                        commands_on_line = 0;
                        trip_first = 0;
                        break;
                    }
                    if (commands_on_line == 5) {
                        commands_on_line = 0;
                        trip_first       = 0;
                        rv = send_and_ensure(socknum);
                    }
                }
            }
            rv = send_and_ensure(socknum);
            dont_send_again = 1;
            break;
        case SHOWROOM:
            lookup_room(room_x, room_y, room_z, socknum);

            // now show the players in room here...
            for (i = 0; i != MAX_CONNS; ++i) {
                if (player[i].in_use == 0 || i == conns || player[i].socknum == 0 || player[num].socknum == 0) {
                    break;
                }
                if (player[i].socknum != player[num].socknum) {
                    if (player[i].in_use == 1 &&
                        get_player_coord(X_COORD_REQUEST, i) == room_x &&
                        get_player_coord(Y_COORD_REQUEST, i) == room_y &&
                        get_player_coord(Z_COORD_REQUEST, i) == room_z) {
                        if (strlen(buf) == 0) {
                            strcpy((char*)buf, player[i].pname); 
                            strcat((char*)buf, " is here too.\n");
                        }
                    }
                }
            }
            if (strlen(buf) != 0) {
                rv = send_and_ensure(socknum);
            }

            strncpy((char*)buf, "Exits:", 6);
            if (map.north == 1) {
                strncat((char*)buf, " NORTH", 7);
            } 
            if (map.south == 1) {
                strncat((char*)buf, " SOUTH", 6);
            } 
            if (map.east == 1) {
                strncat((char*)buf, " EAST", 5);
            } 
            if (map.west == 1) {
                strncat((char*)buf, " WEST", 5);
            } 
            if (map.up == 1) {
                strncat((char*)buf, " U", 2);
            } 
            if (map.down == 1) {
                strncat((char*)buf, " D", 2);
            } 
            if (map.northeast == 1) {
                strncat((char*)buf, " NE", 3);
            } 
            if (map.southeast == 1) {
                strncat((char*)buf, " SE", 3);
            } 
            if (map.southwest == 1) {
                strncat((char*)buf, " SW", 3);
            } 
            if (map.northwest == 1) {
                strncat((char*)buf, " NW", 3);
            } 
            if (strlen(buf) == 6) {
                strncat((char*)buf, " NONE", 5);
            }
            strncat((char*)buf, "\n", 1);
            rv = send_and_ensure(socknum);
            dont_send_again = 1;
            break;
        case INVALDIR:
            strcpy((char*)buf, "Cannot move in that direction. Type 'look' to view current location.");
            break;
        case REQUEST_PW_FOR_NEW:
            strcpy((char*)buf, "You've provided the name [");
            strcat((char*)buf, player[num].pname);
            strcat((char*)buf, "].\n");
            strcat((char*)buf, "Please provide a password less than ");
            snprintf(PWLEN_TMP, 3, "%d", NAMES_MAX);
            strcat((char*)buf, PWLEN_TMP);
            strcat((char*)buf, " characters long.\n");
            break;
        case REQUEST_PW_CONFIRM:
            strcpy((char*)buf, "Please confirm your password by typing it out once more.\n");
            break;
        case REQUEST_PW_FOR_EXISTING:
            strcpy((char*)buf, "Please provide your password.");
            break;
        case ATTEMPT_CREATE_USR:
            strcpy((char*)buf, "Attempting to create your character...\n");
            break;
        case MISMATCH_PW_SET:
            strcpy((char*)buf, "Password mismatch. Start over.\nPlease provide a NAME.\n");
            break;
        case PLAYER_CREATION_FAILED:
            strcpy((char*)buf, "Dreadfully sorry, but character creation failed. Goodbye!\n");
            break;
        case PLAYER_ALREADY_ONLINE: 
            strcpy((char*)buf, "That player is already connected. Please provide another name.\n");
            break;
        case INCORRECT_PASSWORD:
            strcpy((char*)buf, "Incorrect password. Restarting.\nPlease provide a NAME.\n");
            break;
        case UNABLE_TO_RETRIEVE_CHAR:
            strcpy((char*)buf, "Couldn't retrieve your character. uh oh.\n");
            break;
        case NAME_UNAVAILABLE:
            strcpy((char*)buf, "That name is unavailable. Try another, fiend.\n");
            break;
        case ALPHANUM_NAMES_ONLY:
            strcpy((char*)buf, "Yeah, no. Alphanumeric names only, smartass. Try another.\n");
            break;
        case NAME_TOO_LONG:
            strcpy((char*)buf, "That name is too long. As stated previously, the limit is ");
            snprintf(names_max, sizeof(NAMES_MAX), "%d", NAMES_MAX);
            strcat((char*)buf, names_max);
            strcat((char*)buf, " characters. Try again.\nPlease provide a NAME.\n");
            break;
        case NAME_TOO_SHORT:
            strcpy((char*)buf, "That name is too short. The minimum is ");
            snprintf(names_max, sizeof(NAMES_MIN), "%d", NAMES_MIN);
            strcat((char*)buf, names_max);
            strcat((char*)buf, " characters. Try again.\nPlease provide a NAME.\n");
            break;

        case PRINT_PROVIDE_NEW_ROOM_NAME:
            strcpy((char*)buf, "Enter a new room name, up to ");
            snprintf(names_max, sizeof(NAMES_MAX), "%d", NAMES_MAX - 1);
            strcat((char*)buf, names_max);
            strcat((char*)buf, " chars in length.\n");
            break;
        case PRINT_PROVIDE_NEW_ROOM_DESC:
            strcpy((char*)buf, "Enter a new room description, up to ");
            snprintf(names_max, sizeof(BUFLEN), "%d", BUFLEN - 1);
            strcat((char*)buf, names_max);
            strcat((char*)buf, " chars in length.\n");
            break;
        case PRINT_CONFIRM_NEW_ROOM_DESC:
            strcpy((char*)buf, "Confirm the new description by typing Y/y. You entered:");
            rv = send_and_ensure(socknum);
            strncpy((char*)buf, player[num].store, BUFLEN);
            rv = send_and_ensure(socknum);
            strcpy((char*)buf, "If this is wrong, type something other than Y/y.");
            rv = send_and_ensure(socknum);
            dont_send_again = 1;
            break;
        case PRINT_CONFIRM_NEW_ROOM_NAME:
            strcpy((char*)buf, "Confirm the new name by typing Y/y. You entered:");
            rv = send_and_ensure(socknum);
            strncpy((char*)buf, player[num].store, NAMES_MAX);
            rv = send_and_ensure(socknum);
            strcpy((char*)buf, "If this is wrong, type something other than Y/y.");
            rv = send_and_ensure(socknum);
            dont_send_again = 1;
            break;
        case PRINT_ADJUSTMENT_SUCCESSFUL:
            strcpy((char*)buf, "Room adjusted successfully. Exiting editor.\n");
            break;
        case PRINT_COULDNT_ADJUST_ROOM:
            strcpy((char*)buf, "Some kind of error occurred. Room adjustment failed. Exiting editor.\n");
            break;
        case PRINT_EXITING_CMD_WAIT:
            strcpy((char*)buf, "Exiting editor - returning you to the (real) world.\n");
            break;
        case PRINT_INSUFFICIENT_PERMISSIONS:
            strcpy((char*)buf, "You don't have permission to do that.\n");
            break;

        case PRINT_ROOM_EXIT_CHANGED:
            strcpy((char*)buf, "Room exit changed.\n");
            break;
        case PRINT_ROOM_FLAG_CHANGED:
            strcpy((char*)buf, "Room flag toggled.\n");
            break;
        case PRINT_ROOM_REMOVAL_CHECK:
            strcpy((char*)buf, "You're trying to delete this room. Are you sure you want to do this?\nType only y/Y to confirm.");
            break;
        case PRINT_ROOM_REMOVAL_CONFIRM:
            strcpy((char*)buf, "Again, confirm room deletion with y/Y - deleting: ");
            strcat((char*)buf, map.rname);
            strcat((char*)buf, ".\n");
            break;
        case PRINT_ROOM_REMOVAL_SUCCESS:
            strcpy((char*)buf, "Room removed successfully.\n");
            break;
        case PRINT_ROOM_REMOVAL_FAILURE:
            strcpy((char*)buf, "Room removal failed.\nThe owner of this room is: ");
            strcat((char*)buf, map.owner);
            strcat((char*)buf, ".\n");
            break;
        case PRINT_ROOM_CREATION_GIVE_DIR:
            strcpy((char*)buf, "Which direction are you trying to create a room in? (from current)\n");
            break;
        case PRINT_ROOM_CREATION_CONFIRMALL:
            strcpy((char*)buf, "You're adding a room in the direction of: ");
            strncpy(names_max, player[num].store, player[num].store_size);
            strcat((char*)buf, names_max);
            strcat((char*)buf, ". Confirm this by typing y/Y, or decline by typing anything else.\n");
            break;
        case PRINT_ROOM_CREATION_CANNOT:
            strcpy((char*)buf, "Room creation couldn't be completed right now.\n");
            break;
        case PRINT_ROOM_CREATION_SUCCESS:
            strcpy((char*)buf, "Room creation complete.\n");
            break;
        case PRINT_ROOM_CREATION_FAILURE:
            strcpy((char*)buf, "A room already exists in that direction. Exiting editor.\n");
            break;
        case PRINT_REMOVED_FROM_ROOM:
            strcpy((char*)buf, "You've been moved from your current room by the system; the owning player may have deleted it.\n");
            break;
        case PRINT_PROVIDE_ROOM_EXIT_NAME:
            strcpy((char*)buf, "Which direction are you trying to toggle exit visibility for?\n");
            break;
        case PRINT_PROVIDE_ROOM_FLAG_NAME:
            strcpy((char*)buf, "Which flag are you trying to toggle?\n");
            break;
        case PRINT_TOGGLED_ROOM_EXIT:
            strcpy((char*)buf, "Room exit visibility toggled. Exiting editor.\n");
            break;
        case PRINT_COULDNT_TOGGLE_EXIT:
            strcpy((char*)buf, "Unable to toggle the exit for that room.\n");
            break;
        case PRINT_COULDNT_EXIT_NO_ROOM:
            strcpy((char*)buf, "There's no room in that direction.\n");
            break;

        default:
            if (DIRS) {
                switch (argument) {
                    case 0:
                        strcpy((char*)buf, "> RETURNING to the origin room.\n");
                        break;
                    case 1:
                        strcpy((char*)buf, "> MOVING north.\n");
                        break;
                    case 2:
                        strcpy((char*)buf, "> MOVING east.\n");
                        break;
                    case 3:
                        strcpy((char*)buf, "> MOVING south.\n");
                        break;
                    case 4:
                        strcpy((char*)buf, "> MOVING west.\n");
                        break;
                    case 5:
                        strcpy((char*)buf, "> MOVING down.\n");
                        break;
                    case 6:
                        strcpy((char*)buf, "> MOVING up.\n");
                        break;
                    case 7:
                        strcpy((char*)buf, "> MOVING NORTHWEST.\n");
                        break;
                    case 8:
                        strcpy((char*)buf, "> MOVING NORTHEAST.\n");
                        break;
                    case 9:
                        strcpy((char*)buf, "> MOVING SOUTHWEST.\n");
                        break;
                    case 10:
                        strcpy((char*)buf, "> MOVING SOUTHEAST.\n");
                        break;
                }
                rv = send_and_ensure(socknum);
                if (argument == 0) {
                    return 0; 
                }
                return 2;
            }
    }
    if (dont_send_again == 0) {
        rv = send_and_ensure(socknum);
    }
    assert(rv != -13); 
    return 1;
}
