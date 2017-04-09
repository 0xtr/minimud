#include "movement_to_vector.h"

int32_t x_movement_to_vector(const struct Command *info)
{
	if (info->subtype == MOVEMENT_EAST || 
			info->subtype == MOVEMENT_NORTHEAST ||
			info->subtype == MOVEMENT_SOUTHEAST) {
		// returning 1
		return 1;
	} else if (info->subtype == MOVEMENT_SOUTHWEST ||
			info->subtype == MOVEMENT_NORTHWEST ||
			info->subtype == MOVEMENT_WEST) {
		return -1;
	}
	printf("x 0\n");
	return 0;
}

int32_t y_movement_to_vector(const struct Command *info)
{
	if (info->subtype == MOVEMENT_NORTH || 
			info->subtype == MOVEMENT_NORTHEAST ||
			info->subtype == MOVEMENT_NORTHWEST) {
		return 1;
	} else if (info->subtype == MOVEMENT_SOUTHEAST || 
			info->subtype == MOVEMENT_SOUTHWEST ||
			info->subtype == MOVEMENT_SOUTH) {
		return -1;
	}
	return 0;
}

int32_t z_movement_to_vector(const struct Command *info)
{
	if (info->subtype == MOVEMENT_UP) {
		return 1;
	} else if (info->subtype == MOVEMENT_DOWN) {
		return -1;
	}
	return 0;
}
