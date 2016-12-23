int32_t parse_clist (void) {
    FILE *fp = NULL;
    uint8_t *line = NULL;
    ssize_t ret = 0;
    size_t linelen = 0;
    int32_t num_of_cmds = 0;

    if (access("src/commands/COMMAND_LIST.txt", F_OK) == -1) {
        perror("No commands defined!");
        exit(EXIT_FAILURE);
    }
    fp = fopen("src/commands/COMMAND_LIST.txt", "r");
    if (fp == NULL) {
        fprintf(stdout, "Couldn't open the command file for parsing.\n");
        exit(EXIT_FAILURE);
    }

    head = NULL;
    for (size_t i = 0; (ret = getline((char*)&line), linelen, fp) != EOF; ++i) {
        if (line[0] != '#' && strlen((char*)line) != 0 &&
            line[0] != '.' && line[0] != '\n') {
            int32_t len = strlen((char*)line);
            if (len > MAX_COMMAND_LEN) {
                MAX_COMMAND_LEN = len;
            }

            ++num_of_cmds;
            curr = (clist*) malloc(sizeof(clist));
            if (ret == 0) {
                free(line);
                fprintf(stdout, "Couldn't read from the command file.\n");
                perror("Couldn't read from the command file.");
            }
            curr->cname = malloc(len + 5);
            memset(curr->cname, '\0', len + 5);
            if (line[strlen((char*)line)] == '.') {
                line[strlen((char*)line)] = '\0';
            }
            if (line[strlen((char*)line) - 1] == '\n') {
                line[strlen((char*)line) - 1] = '\0';
            }
            strcpy((char*)curr->cname, (char*)line);
            //fprintf(stdout, "Added command: %s.\n", curr->cname);
            curr->next = head;
            head = curr;
        }
    }
    curr = head;

    free(line);
    return num_of_cmds;
}


int32_t check_clist (const int32_t socket_num, const uint8_t command[]) {
    int32_t rv, num = getplayernum(socket_num);
    clist *tmp = head; // last in file
    while (tmp) {
        if (strcmp((char*)command, (char*)tmp->cname) == 0) {
            if (is_direction(command[0])) {
                rv = move_player(socket_num, command); 
                if (rv != -1) {
                    rv = print_output(socket_num, SHOWROOM);
                }
            }
            // isroomcmd
            // isgamecmd
            if (strcmp((char*)command, "look") == 0 ||
                       strcmp((char*)command, "l")    == 0) {
                rv = print_output(socket_num, SHOWROOM);
            } else if (strcmp((char*)command, "say") == 0 ||
                       strcmp((char*)command, "Say") == 0) {
                rv = print_player_speech (socket_num, command);
            } else if (strcmp((char*)command, "quit") == 0) {
                rv = shutdown_socket(num);
            } else if (strcmp((char*)command, "commands") == 0) {
                rv = print_output(socket_num, SHOWCMDS); 
            } else if (strcmp((char*)command, "return") == 0) {
                rv = move_player(socket_num, command);
                rv = print_output(socket_num, SHOWROOM);
            } else if (strcmp((char*)command, "players") == 0) {
                rv = print_output(socket_num, LISTPLAYERS);

            ////////// ROOMS //////////
            // if (issetcmd(command[0]) == 0) {
            //     set_room(socket_num, command);
            // }
            } else if (strcmp((char*)command, "setrname") == 0) {
                rv = print_output(socket_num, PRINT_PROVIDE_NEW_ROOM_NAME);
                player[num].wait_state = WAIT_ENTER_NEW_ROOM_NAME;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrdesc") == 0) {
                rv = print_output(socket_num, PRINT_PROVIDE_NEW_ROOM_DESC);
                player[num].wait_state = WAIT_ENTER_NEW_ROOM_DESC;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrexit") == 0) {
                rv = print_output(socket_num, PRINT_PROVIDE_ROOM_EXIT_NAME);
                player[num].wait_state = WAIT_ENTER_EXIT_NAME;
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "setrflag") == 0) {
                rv = print_output(socket_num, PRINT_PROVIDE_ROOM_FLAG_NAME);
                player[num].wait_state = WAIT_ENTER_FLAG_NAME;
                player[num].hold_for_input = 1;
            // if (isroomexistcmd(command[0]) == 0) {
            //     something
            // }
            } else if (strcmp((char*)command, "mkroom") == 0) {
                player[num].wait_state = WAIT_ROOM_CREATION_DIR;
                rv = print_output(socket_num, PRINT_ROOM_CREATION_GIVE_DIR);
                player[num].hold_for_input = 1;
            } else if (strcmp((char*)command, "rmroom") == 0) {
                player[num].wait_state = WAIT_ROOM_REMOVAL_CHECK;
                rv = print_output(socket_num, PRINT_ROOM_REMOVAL_CHECK);
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
