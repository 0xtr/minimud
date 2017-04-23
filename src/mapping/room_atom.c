#include "room_atom.h"

struct room_atom *get_room(void)
{
	return (struct room_atom *)calloc(sizeof(struct room_atom), sizeof(struct room_atom));
}
