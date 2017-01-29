static const uint8_t RESERVED_WORDS[10][10] = { "admin", "root", "TABLE", "ADMIN", "Admin", "Administrator" };

int32_t check_if_name_is_reserved (const int32_t pnum, const uint8_t *name) {
    for (size_t i = 0; i < 10; ++i) {
        if (strcmp(name, (char*)RESERVED_WORDS[i]) == 0) {
            print_output(pnum, NAME_UNAVAILABLE);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int32_t check_if_name_is_valid (const int32_t pnum, const uint8_t *name) {
    if (strlen((char*)get_player_buffer(pnum)) > NAMES_MAX || strlen((char*)name) < NAMES_MIN) {
        print_output(pnum, NAME_NOT_WITHIN_PARAMS);
        return EXIT_FAILURE;
    }
    for (int32_t i = 0; i < NAMES_MAX; ++i) {
        if (!isalpha(get_player_buffer(pnum)[i])) {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
