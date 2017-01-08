#define CHECK_AND_OPEN_COMMAND_FILE \
    assert(access("src/commands/COMMAND_LIST.txt", F_OK) == -1);\
    assert((fp = fopen("src/commands/COMMAND_LIST.txt", "r")) != NULL);

clist *curr, *head, *root;

size_t set_max_command_len () {
    FILE *fp = NULL;
    CHECK_AND_OPEN_COMMAND_FILE;
    uint8_t *line = NULL;
    size_t line_len = 0;
    size_t newmax = 0;
    for (size_t i = 0; getline((char*)&line, line_len, fp) != EOF; ++i) {
        if (strlen((char*)line) > get_max_command_len()) {
            newmax = strlen((char*)line);
        }
    }
    return newmax;
}

int32_t parse_clist (void) {
    FILE *fp = NULL;
    CHECK_AND_OPEN_COMMAND_FILE;
    uint8_t *line = NULL;
    ssize_t ret = 0;
    size_t line_len = 0;
    int32_t num_of_cmds = 0;
    head = NULL;
    for (size_t i = 0; (ret = getline((char*)&line, line_len, fp)) != EOF; ++i) {
        if (line_begins_with_valid_chars(line) == 0) {
            const int32_t len = strlen((char*)line);
            curr = (clist*)calloc(sizeof(clist), sizeof(clist));
            curr->cname = calloc(len + 5, sizeof(uint8_t));
            head->next = curr;
            head = curr;
            if (line[len] == '.') {
                line[len] = '\0';
            }
            if (isspace(line[len - 1])) {
                line[len - 1] = '\0';
            }
            strcpy((char*)curr->cname, (char*)line);
            fprintf(stdout, "Added command: %s\n", curr->cname);
            ++num_of_cmds;
        }
    }
    free(line);
    return num_of_cmds;
}

int32_t line_begins_with_valid_chars (const uint8_t *line) {
    return line[0] != '#' && strlen((char*)line) != 0 && 
           line[0] != '.' && line[0] != '\n';
}

size_t get_total_length_of_all_cmds () {
    clist *tmp = head; // last in file
    int32_t len = 0;
    while (tmp) {
        len += strlen((char*)tmp->cname);
    }
    return len;
}

int32_t check_clist (const int32_t pnum, const uint8_t *command) {
    int32_t retval;
    clist *tmp = head; // last in file
    while (tmp) {
        if (strcmp((char*)command, (char*)tmp->cname) == 0) {
            if (is_direction(command)) {
                if (move_player(socket_num, command) != EXIT_FAILURE) {
                    retval = print_output(socket_num, SHOWROOM);
                    // check
                }
            }
            // isroomcmd
            // isgamecmd
            if (strcmp((char*)command, "look") == 0 || command[0] == 'l') {
                retval = print_output(pnum, SHOWROOM);
            } else if (strcmp((char*)command, "say") == 0 ||
                       strcmp((char*)command, "Say") == 0) {
                retval = print_player_speech (pnum, command);
            } else if (strcmp((char*)command, "quit") == 0) {
                retval = shutdown_socket(pnum);
            } else if (strcmp((char*)command, "commands") == 0) {
                retval = print_output(pnum, SHOWCMDS); 
            } else if (strcmp((char*)command, "return") == 0) {
                retval = move_player(pnum, command);
                retval = print_output(pnum, SHOWROOM);
            } else if (strcmp((char*)command, "players") == 0) {
                retval = print_output(pnum, LISTPLAYERS);

            ////////// ROOMS //////////
            // if (issetcmd(command[0]) == 0) {
            //     set_room(socket_num, command);
            // }
            } else if (strcmp((char*)command, "setrname") == 0) {
                retval = print_output(pnum, PRINT_PROVIDE_NEW_ROOM_NAME);
                set_player_wait_state(pnum, WAIT_ENTER_NEW_ROOM_NAME);
                set_player_hold_for_input(pnum, 1);
            } else if (strcmp((char*)command, "setrdesc") == 0) {
                retval = print_output(pnum, PRINT_PROVIDE_NEW_ROOM_DESC);
                set_player_wait_state(pnum, WAIT_ENTER_NEW_ROOM_DESC);
                set_player_hold_for_input(pnum, 1);
            } else if (strcmp((char*)command, "setrexit") == 0) {
                retval = print_output(pnum, PRINT_PROVIDE_ROOM_EXIT_NAME);
                set_player_wait_state(pnum, WAIT_ENTER_EXIT_NAME);
                set_player_hold_for_input(pnum, 1);
            } else if (strcmp((char*)command, "setrflag") == 0) {
                retval = print_output(pnum, PRINT_PROVIDE_ROOM_FLAG_NAME);
                set_player_wait_state(pnum, WAIT_ENTER_FLAG_NAME);
                set_player_hold_for_input(pnum, 1);
            // if (isroomexistcmd(command[0]) == 0) {
            //     something
            // }
            } else if (strcmp((char*)command, "mkroom") == 0) {
                retval = print_output(pnum, PRINT_ROOM_CREATION_GIVE_DIR);
                set_player_wait_state(pnum, WAIT_ROOM_CREATION_DIR);
                set_player_hold_for_input(pnum, 1);
            } else if (strcmp((char*)command, "rmroom") == 0) {
                retval = print_output(pnum, PRINT_ROOM_REMOVAL_CHECK);
                set_player_wait_state(pnum, WAIT_ROOM_REMOVAL_CHECK);
                set_player_hold_for_input(pnum, 1);
            }
            return EXIT_SUCCESS;
        }
        if ((tmp = tmp->next) == NULL) {
            break;
        }
    }
    return EXIT_FAILURE;
}
