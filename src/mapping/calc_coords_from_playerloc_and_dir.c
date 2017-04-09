#include "calc_coords_from_playerloc_and_dir.h"

struct Coordinates calc_coords_from_playerloc_and_dir(const int32_t socket)
{
	struct Coordinates coords = get_player_coords(socket);
	printf("calc %d %d %d\n", coords.x, coords.y, coords.z);

	if (get_player_store(socket) == NULL || strlen((char *)get_player_store(socket)) == 0) {
		coords.x = coords.y = coords.z = -1;
		return coords;
	}

	struct Command *info = (struct Command *)malloc(sizeof(struct Command));
	info->type = COMMAND_NOT;
	info->subtype = COMMAND_NOT;
	info = is_movement_cmd(get_player_store(socket), info);

	printf("info: type %d sub %d\n", info->type, info->subtype);

	coords.x += x_movement_to_vector(info);
	coords.y += y_movement_to_vector(info);
	coords.z += z_movement_to_vector(info);

	free(info);

	printf("calc %d %d %d\n", coords.x, coords.y, coords.z);
	return coords;
}
