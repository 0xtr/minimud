#include "calc_coords_from_playerloc_and_dir.h"

struct coordinates calc_coords_from_playerloc_and_dir(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);

	if (player->store == NULL || strlen((char *)player->store) == 0) {
		coords.x = coords.y = coords.z = -1;
		return coords;
	}

	struct command *info = (struct command *)calloc(sizeof(struct command), 
			sizeof(struct command));

	info->type = COMMAND_NOT;
	info->subtype = COMMAND_NOT;

	info = is_movement_cmd(player->store, info);

	coords.x += x_movement_to_vector(info);
	coords.y += y_movement_to_vector(info);
	coords.z += z_movement_to_vector(info);

	free(info);

	return coords;
}

int32_t x_movement_to_vector(const struct command *info)
{
	if (info->subtype == DIR_EAST || 
			info->subtype == DIR_NORTHEAST ||
			info->subtype == DIR_SOUTHEAST) {
		// returning 1
		return 1;
	} else if (info->subtype == DIR_SOUTHWEST ||
			info->subtype == DIR_NORTHWEST ||
			info->subtype == DIR_WEST) {
		return -1;
	}
	return 0;
}

int32_t y_movement_to_vector(const struct command *info)
{
	if (info->subtype == DIR_NORTH || 
			info->subtype == DIR_NORTHEAST ||
			info->subtype == DIR_NORTHWEST) {
		return 1;
	} else if (info->subtype == DIR_SOUTHEAST || 
			info->subtype == DIR_SOUTHWEST ||
			info->subtype == DIR_SOUTH) {
		return -1;
	}
	return 0;
}

int32_t z_movement_to_vector(const struct command *info)
{
	if (info->subtype == DIR_UP) {
		return 1;
	} else if (info->subtype == DIR_DOWN) {
		return -1;
	}
	return 0;
}
