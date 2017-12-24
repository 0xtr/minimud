#include "room_db_record.h"

struct room_db_record *get_room(void)
{
	return (struct room_db_record *)calloc(sizeof(struct room_db_record), sizeof(struct room_db_record));
}
