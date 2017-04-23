#include "calc_coords_from_playerloc_and_dir.h"

struct coordinates calc_coords_from_playerloc_and_dir(const int32_t socket)
{
	struct coordinates coords = get_player_coords(socket);

	if (get_player_store(socket) == NULL || strlen((char *)get_player_store(socket)) == 0) {
		coords.x = coords.y = coords.z = -1;
		return coords;
	}

	struct command *info = (struct command *)malloc(sizeof(struct command));

	info->type = COMMAND_NOT;
	info->subtype = COMMAND_NOT;

	info = is_movement_cmd(get_player_store(socket), info);

	coords.x += x_movement_to_vector(info);
	coords.y += y_movement_to_vector(info);
	coords.z += z_movement_to_vector(info);

	free(info);

	return coords;
}
