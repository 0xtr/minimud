int32_t shutdown_socket (const int32_t playernum) {
    if (shutdown(player[playernum].socket_num, SHUT_RDWR) == -1) {
        if (errno != ENOTCONN) {
            log_issue((uint8_t*)"Failed to shutdown a connection.\n");
            return EXIT_FAILURE;
        }
    }
    player[playernum].in_use = 0;
    player[playernum].socket_num = 0;
    decrement_active_conns();
    return EXIT_SUCCESS;
}
