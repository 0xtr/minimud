#include "common.h"
#include "Map.h"

struct Map *get_room(void)
{
	struct Map *map = (struct Map *)calloc(sizeof(struct Map), sizeof(struct Map));

	return map;
}

void free_room(struct Map *map)
{
	free(map);
}
