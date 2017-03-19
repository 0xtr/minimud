#include "RoomRecord.h"

struct RoomRecord *get_room(void)
{
	return (struct RoomRecord *)calloc(sizeof(struct RoomRecord), sizeof(struct RoomRecord));
}

void free_room(struct RoomRecord *map)
{
	free(map);
}
