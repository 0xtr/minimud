int32_t print_player_speech (const int32_t socket_num, const uint8_t *say) {
    #define TOKEN_SAY_CMD_LEN 4 // length req'd for the actual say command + the space after that
    #define TOKEN_YOU_SAY_LEN 9 // length req'd for player to see You say: 
    uint8_t *buffer = calloc(BUFFER_LENGTH, sizeof(uint8_t));
    const int32_t pnum = getplayernum(socket_num);

    strncpy(buffer, player[pnum].pname, strlen(player[pnum].pname));
    strncat(buffer, " says: ", 7);
    strncat(buffer, &say[TOKEN_SAY_CMD_LEN], BUFFER_LENGTH - TOKEN_SAY_CMD_LEN);
    echoaround_player(socket_num, ROOM_ONLY, buffer);
#ifdef DEBUG
    printf("print_player_speech: %s\n", buffer);
#endif
    free(buffer);

    set_player_buffer_replace(pnum, (uint8_t*)"You say: ");
    set_player_buffer_append(buffer, &say[TOKEN_SAY_CMD_LEN]);
    assert(send_and_ensure(pnum, NULL) == EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
