#include "../common.h"
#include "echoaround_player.h"

int32_t echoaround_player(const int32_t pnum, const uint8_t *buffer, const int32_t location)
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
					assert(outgoing_msg_handler(i) == EXIT_SUCCESS);
				}
			} else if (location == ALL_PLAYERS) {
				set_player_buffer_replace(i, buffer);
				assert(outgoing_msg_handler(i) == EXIT_SUCCESS);
			}
		}
	}

	return EXIT_SUCCESS;
}
