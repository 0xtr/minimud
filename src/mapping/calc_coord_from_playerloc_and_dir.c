#include "calc_coord_from_playerloc_and_dir.h"

int32_t calc_coord_from_playerloc_and_dir(const int32_t coord_type, const int32_t socket)
{
	int32_t x, y, z;
	x = get_player_coord(X_COORD_REQUEST, socket);
	y = get_player_coord(Y_COORD_REQUEST, socket);
	z = get_player_coord(Z_COORD_REQUEST, socket);

	if (get_player_store(socket) == NULL || strlen((char *)get_player_store(socket)) == 0)
		return EXIT_FAILURE;

	uint8_t tmp[BUFFER_LENGTH] = {0};
	memcpy(tmp, get_player_store(socket), strlen((char *)get_player_store(socket)));

	for (size_t i = 0; i < strlen((char *)tmp); ++i) {
		if (isalpha(tmp[i]) == 0) {
			tmp[i] = '\0';
			break;
		}
	}

	if (coord_type == X_COORD_REQUEST) {
		return (x += x_movement_to_vector(tmp));
	} else if (coord_type == Y_COORD_REQUEST) {
		return (y += y_movement_to_vector(tmp));
	} else if (coord_type == Z_COORD_REQUEST) {
		return (z += z_movement_to_vector(tmp));
	}

	return EXIT_FAILURE;
}
