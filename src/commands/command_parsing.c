static int32_t MAX_COMMAND_LEN = set_max_command_len();
#define CHECK_AND_OPEN_COMMAND_FILE \
    assert(access("src/commands/COMMAND_LIST.txt", F_OK) == -1);\
    assert((fp = fopen("src/commands/COMMAND_LIST.txt", "r")) != NULL);

int32_t get_max_command_len () {
    return MAX_COMMAND_LEN;
}

int32_t set_max_command_len () {
    FILE *fp = NULL;
    CHECK_AND_OPEN_COMMAND_FILE;
    uint8_t *line = NULL;
    size_t line_len = 0;
    int32_t newmax = 0;
    for (size_t i = 0; getline((char*)&line, line_len, fp) != EOF; ++i) {
        int32_t len = strlen((char*)line);
        if (len > MAX_COMMAND_LEN) {
            newmax = len;
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
            curr = (clist*)malloc(sizeof(clist));
            curr->cname = malloc(len + 5);
            memset(curr->cname, '\0', len + 5);
            if (line[strlen((char*)line)] == '.') {
                line[strlen((char*)line)] = '\0';
            }
            if (line[strlen((char*)line) - 1] == '\n') {
                line[strlen((char*)line) - 1] = '\0';
            }
            strcpy((char*)curr->cname, (char*)line);
            fprintf(stdout, "Added command: %s\n", curr->cname);
            ++num_of_cmds;
            curr->next = head;
            head = curr;
        }
    }
    curr = head;
    free(line);
    return num_of_cmds;
}

int32_t line_begins_with_valid_chars (const uint8_t *line) {
    return line[0] != '#' && strlen((char*)line) != 0 && 
           line[0] != '.' && line[0] != '\n';
}

int32_t check_clist (const int32_t socket_num, const uint8_t command[]) {
    int32_t retval, num = getplayernum(socket_num);
    clist *tmp = head; // last in file
    while (tmp) {
        if (strcmp((char*)command, (char*)tmp->cname) == 0) {
            if (is_direction(command)) {
                retval = move_player(socket_num, command); 
                if (retval != -1) {
                    retval = print_output(socket_num, SHOWROOM);
                }
            }
            // isroomcmd
            // isgamecmd
            if (strcmp((char*)command, "look") == 0 ||
                       strcmp((char*)command, "l")    == 0) {
                retval = print_output(socket_num, SHOWROOM);
            } else if (strcmp((char*)command, "say") == 0 ||
                       strcmp((char*)command, "Say") == 0) {
                retval = print_player_speech (socket_num, command);
            } else if (strcmp((char*)command, "quit") == 0) {
                retval = shutdown_socket(num);
            } else if (strcmp((char*)command, "commands") == 0) {
                retval = print_output(socket_num, SHOWCMDS); 
            } else if (strcmp((char*)command, "return") == 0) {
                retval = move_player(socket_num, command);
                retval = print_output(socket_num, SHOWROOM);
            } else if (strcmp((char*)command, "players") == 0) {
                retval = print_output(socket_num, LISTPLAYERS);

            ////////// ROOMS //////////
            // if (issetcmd(command[0]) == 0) {
            //     set_room(socket_num, command);
            // }
            } else if (strcmp((char*)command, "setrname") == 0) {
                retval = print_output(socket_num, PRINT_PROVIDE_NEW_ROOM_NAME);
                player[num].wait_state = WAIT_ENTER_NEW_ROOM_NAME;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrdesc") == 0) {
                retval = print_output(socket_num, PRINT_PROVIDE_NEW_ROOM_DESC);
                player[num].wait_state = WAIT_ENTER_NEW_ROOM_DESC;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrexit") == 0) {
                retval = print_output(socket_num, PRINT_PROVIDE_ROOM_EXIT_NAME);
                player[num].wait_state = WAIT_ENTER_EXIT_NAME;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrflag") == 0) {
                retval = print_output(socket_num, PRINT_PROVIDE_ROOM_FLAG_NAME);
                player[num].wait_state = WAIT_ENTER_FLAG_NAME;
                player[num].hold_for_input = 1;
            // if (isroomexistcmd(command[0]) == 0) {
            //     something
            // }
            } else if (strcmp((char*)command, "mkroom") == 0) {
                player[num].wait_state = WAIT_ROOM_CREATION_DIR;
                retval = print_output(socket_num, PRINT_ROOM_CREATION_GIVE_DIR);
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "rmroom") == 0) {
                player[num].wait_state = WAIT_ROOM_REMOVAL_CHECK;
                retval = print_output(socket_num, PRINT_ROOM_REMOVAL_CHECK);
                player[num].hold_for_input = 1;
            }
            return EXIT_SUCCESS;
        }
        if ((tmp = tmp->next) == NULL) {
            break;
        }
    }
    return EXIT_FAILURE;
}
