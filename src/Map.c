#include "common.h"
#include "Map.h"

struct Map *get_room(void)
{
	return (struct Map *)calloc(sizeof(struct Map), sizeof(struct Map));
}

void free_room(struct Map *map)
{
	free(map);
}
