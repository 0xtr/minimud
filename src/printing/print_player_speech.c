#include "../common.h"
#include "print_player_speech.h"

int32_t print_player_speech (const int32_t pnum, const uint8_t *say) {
    #define TOKEN_SAY_CMD_LEN 4 // length req'd for the actual say command + the space after that
    #define TOKEN_YOU_SAY_LEN 9 // length req'd for player to see You say: 
    uint8_t *buffer = calloc(BUFFER_LENGTH, sizeof(uint8_t));

    strncpy((char *)buffer, (char *)get_player_pname(pnum), strlen((char *)get_player_pname(pnum)));
    strncat((char *)buffer, " says: ", BUFFER_LENGTH - strlen((char *)buffer));
    strncat((char *)buffer, (char *)say[TOKEN_SAY_CMD_LEN], BUFFER_LENGTH - strlen((char *)buffer));

    echoaround_player(pnum, buffer, ROOM_ONLY);
#ifdef DEBUG
    printf("print_player_speech: %s\n", buffer);
#endif

    free(buffer);

    set_player_buffer_replace(pnum, (uint8_t*)"You say: ");
    set_player_buffer_append(pnum, &say[TOKEN_SAY_CMD_LEN]);

    assert(outgoing_msg_handler(pnum) == EXIT_SUCCESS);

    return EXIT_SUCCESS;
}
