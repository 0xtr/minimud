int32_t shutdown_socket (const int32_t playernum) {
    if (shutdown(player[playernum].socknum, SHUT_RDWR) == -1) {
        if (errno != ENOTCONN) {
            log_issue((uint8_t*)"Failed to shutdown a connection.\n");
            return EXIT_FAILURE;
        }
    }
    strcpy(buf, (uint8_t*)player[playernum].pname); 
    strcat(buf, (uint8_t*)" has disconnected.");

    for (size_t i = 0; i <= get_active_conns(); ++i) {
        if (i != playernum) {
            if (player[i].socknum != player[num].socknum) {
                if (player[i].in_use == 1 && player[playernum].in_use == 1) {
                    wfd = player[i].socknum;
                    rv = send_and_ensure(i);
                    if (rv != 1) {
                        log_issue((uint8_t*)"Problem sending say data.");
                        break;
                    }
                }
            }
        }
    }
    player[playernum].in_use = 0;
    player[playernum].socknum = 0;
    --conns;
    return EXIT_SUCCESS;
}
