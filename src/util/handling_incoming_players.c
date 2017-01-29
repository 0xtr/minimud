int32_t check_for_highest_socket_num (void) {
    int32_t fdmax = 0;
    for (size_t j = 0; j < get_num_of_players(); ++j) {
        if (get_player_socket(j) > 0) {
            //FD_SET(get_player_socket(j), &rfds);
            if (get_player_socket(j) > fdmax) {
                fdmax = get_player_socket(j);
            }
        }
    }
    return fdmax;
}

int32_t check_if_player_is_already_online (const int32_t pnum) {
    for (size_t i = 0; i < get_num_of_players(); ++i) {
        if (strcmp((char*)get_player_pname(pnum), (char*)get_player_pname(i)) == 0 && i != pnum) {
            print_output(pnum, PLAYER_ALREADY_ONLINE);
            set_player_wait_state(pnum, THEIR_NAME);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int32_t find_and_set_new_player_struct (const int32_t wfd) {
    int32_t set = 0;
    for (size_t j = 0;; ++j) {
        if (get_player_in_use(j) != 0) {
            continue;
        }
        set_player_in_use(j, true);
        set_player_hold_for_input(j, true);
        set_player_socket(j, wfd);
        set_player_wait_state(j, THEIR_NAME);
        FD_SET(wfd, &rfds);
        if (wfd > fdmax) {
            fdmax = wfd;
        }
        set = 1;
        set_active_connections();

        /* this doesn't actually do anything productive right now
         * returns a local address. maybe because telnet to localhost
         * or maybe because doing it wrong. to come back to later. 
         * might be useful if working in future to pass a single IP
         * to bind instead of 0.0.0.0 */
        struct sockaddr_in addr;
        //socklen_t addr_size = sizeof(struct sockaddr_in); 
        struct ifaddrs *ifap;
        //status = getifaddrs(&ifap);
        //status = getpeername(wfd, player[i].address, &player[i].address_len);
        //strncpy(player[j].holder, inet_ntoa(addr.sin_addr), NAMES_MAX);
        break;
    }
    return set;
}

int32_t ensure_player_moving_valid_dir (const int32_t pnum, const uint8_t *command) {
    if (is_direction(command) == EXIT_FAILURE) {
        print_output(pnum, INVALDIR);\
        print_output(pnum, PRINT_EXITING_CMD_WAIT);\
        set_player_wait_state(pnum, NO_WAIT_STATE);
        set_player_hold_for_input(pnum, 0);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
