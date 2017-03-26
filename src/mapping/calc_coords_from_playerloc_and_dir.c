#include "calc_coords_from_playerloc_and_dir.h"

struct Coordinates calc_coords_from_playerloc_and_dir(const int32_t socket)
{
	struct Coordinates coords = get_player_coords(socket);

	if (get_player_store(socket) == NULL || strlen((char *)get_player_store(socket)) == 0) {
		coords.x = coords.y = coords.z = -1;
		return coords;
	}

	uint8_t tmp[BUFFER_LENGTH] = {0};
	memcpy(tmp, get_player_store(socket), strlen((char *)get_player_store(socket)));

	for (size_t i = 0; i < strlen((char *)tmp); ++i) {
		if (isalpha(tmp[i]) == 0) {
			tmp[i] = '\0';
			break;
		}
	}

	coords.x += x_movement_to_vector(tmp);
	coords.y += y_movement_to_vector(tmp);
	coords.z += z_movement_to_vector(tmp);

	return coords;
}
