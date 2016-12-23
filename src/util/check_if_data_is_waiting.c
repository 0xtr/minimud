int32_t check_if_data_is_waiting (const int32_t fdmax, const fd_set rfds) {
    int32_t wfd = 0;
    for (size_t k = 0; k <= fdmax; ++k) {
        wfd = player[k].socknum;
        if (FD_ISSET(wfd, &rfds)) {
            process_data(wfd);
            break;
        }
    }
    return EXIT_SUCCESS;
}
