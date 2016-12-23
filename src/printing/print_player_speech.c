int32_t *print_player_speech (int32_t socket_num, const uint8_t *say) {
    // length req'd for the actual command + the space after that
    #define AFTER_SAY_AND_COLON 4
    // length req'd to add 'SAY: ' at start of print32_t to that room
    #define SAY_LEN             7
    // length req'd for player to see You say: 
    #define PLAYER_SAY          9

    /* ALLOCATE x2 */
    uint8_t *tmp_buf = calloc(BUFLEN, sizeof(uint8_t);
    uint8_t *buf_for_player = calloc(BUFLEN, sizeof(uint8_t));
    // do stuff here
    /* end */

    int32_t pnum = getplayernum(socket_num);
    strncpy(tmp_buf, &say[AFTER_SAY_AND_COLON], BUFLEN - AFTER_SAY_AND_COLON);

    // and here
    strncpy(buf, player[pnum].pname, strlen(player[pnum].pname));
    strncat(buf, " says: ", SAY_LEN);
    strncat(buf, tmp_buf, BUFLEN - strlen(buf));

    printf("print_player_speech: %s\n", buf);
    strcpy(buf_for_player, "You say: ");
    strncat(buf_for_player, tmp_buf, BUFLEN - PLAYER_SAY);

    echoaround_player(pnum, ROOM_ONLY);
    //wfd = player[pnum].socket_num;
    strncpy(buf, buf_for_player, BUFLEN);
    send_and_ensure(socket_num);

    /* FREE X2 */
    free(tmp_buf);
    free(buf_for_player);
    /* end */

    return EXIT_SUCCESS;
}
