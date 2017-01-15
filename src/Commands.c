static size_t MAX_COMMAND_LEN = set_max_command_len();
static size_t NUM_OF_AVAILABLE_COMMANDS  = get_num_of_available_cmds();

size_t get_max_command_len () {
    return MAX_COMMAND_LEN;
}

size_t get_num_of_available_cmds () {
    return NUM_OF_AVAILABLE_COMMANDS;
}
