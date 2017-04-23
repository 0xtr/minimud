#include "movement_to_vector.h"

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
