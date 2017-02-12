int32_t check_if_data_is_waiting (const fd_set rfds) {
    for (size_t k = 0; k <= get_num_of_players(); ++k) {
        if (FD_ISSET(get_player_socket(k), &rfds)) {
            process_data(get_player_socket(k));
        }
    }
    return EXIT_SUCCESS;
}
