int32_t write_greeting (const int32_t pnum) {
    set_player_buffer_replace(pnum, "> WELCOME.\n\n");
    set_player_buffer_append(pnum, (uint8_t*)"Please provide a NAME; this can be two words and up to ");
    set_player_buffer_append(pnum, (uint8_t)NAMES_MAX);
    set_player_buffer_append(pnum, (uint8_t*)" characters long in total.\nIf you've already created a character, enter your previous name to resume.");
    assert(outgoing_msg_handler(pnum) == EXIT_SUCCESS);
    return 0;
}
