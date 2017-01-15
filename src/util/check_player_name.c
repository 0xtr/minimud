int32_t check_if_name_is_reserved (const int32_t pnum, const uint8_t *name) {
    clist *tmp = head;
    while (tmp) {
        if ((strcmp(command, tmp->cname) == 0) || (strcmp(command, "TABLE") == 0) ||
            (strcmp(command, "admin") == 0)    || (strcmp(command, "root") == 0)) {
            print_output(pnum, NAME_UNAVAILABLE);
            return EXIT_FAILURE;
        }
        tmp = tmp->next;
    }
    return EXIT_SUCCESS;
}

int32_t check_if_name_is_valid (const int32_t pnum, const uint8_t *name) {
    if (strlen(get_player_buffer(pnum)) > NAMES_MAX || strlen(name) < NAMES_MIN) {
        print_output(socknum, NAME_NOT_WITHIN_PARAMS);
    }
    for (int32_t i = 0; i < NAMES_MAX; ++i) {
        if (!isalpha(get_player_buffer(pnum)[i])) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
