#include "print_not_player.h"

int32_t print_not_player(const int32_t pnum, const uint8_t *buffer, const int32_t location)
{
	int32_t X = get_player_coord(X_COORD_REQUEST, pnum);
	int32_t Y = get_player_coord(Y_COORD_REQUEST, pnum);
	int32_t Z = get_player_coord(Z_COORD_REQUEST, pnum);

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (get_player_socket(i) != get_player_socket(pnum)) {
			if (location == ROOM_ONLY) {
				if (get_player_coord(X_COORD_REQUEST, i) == X &&
					get_player_coord(Y_COORD_REQUEST, i) == Y &&
					get_player_coord(Z_COORD_REQUEST, i) == Z) {
					set_player_buffer_replace(i, buffer);
					assert(outgoing_handler(i) == EXIT_SUCCESS);
				}
			} else if (location == ALL_PLAYERS) {
				set_player_buffer_replace(i, buffer);
				assert(outgoing_handler(i) == EXIT_SUCCESS);
			}
		}
	}

	return EXIT_SUCCESS;
}

int32_t print_player_speech_to_others(const int32_t pnum, const uint8_t *say)
{
	#define TOKEN_SAY_CMD_LEN 4 // length req'd for the actual say command + the space after that
	#define TOKEN_YOU_SAY_LEN 9 // length req'd for player to see You say: 
	uint8_t *buffer = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	strncpy((char *)buffer, (char *)get_player_pname(pnum), strlen((char *)get_player_pname(pnum)));
	strncat((char *)buffer, " says: ", BUFFER_LENGTH - strlen((char *)buffer));
	strncat((char *)buffer, (char)&say[TOKEN_SAY_CMD_LEN], BUFFER_LENGTH - strlen((char *)buffer));

	print_not_player(pnum, buffer, ROOM_ONLY);
#ifdef DEBUG
	printf("print_player_speech: %s\n", buffer);
#endif

	free(buffer);

	set_player_buffer_replace(pnum, (uint8_t*)"You say: ");
	set_player_buffer_append(pnum, &say[TOKEN_SAY_CMD_LEN]);

	assert(outgoing_handler(pnum) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}
