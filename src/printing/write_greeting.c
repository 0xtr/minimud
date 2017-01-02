int32_t write_greeting (const int32_t player_num) {
    sprintf(player[player_num].buffer, (uint8_t*)"%s", "> WELCOME.");
    send_and_ensure(player_num);
    strcat(player[player_num].buffer, 
          (uint8_t*)"\n\nPlease provide a NAME; this can be two words and up to ");
    uint8_t names_max[sizeof(NAMES_MAX)];
    snprintf(names_max, sizeof(NAMES_MAX), "%d", NAMES_MAX);
    strcat(player[player_num].buffer, names_max);
    strcat(player[player_num].buffer, 
           " characters long in total.\nIf you've already created a character, enter your previous name to resume.");
    send_and_ensure(player_num);
    return 0;
}
