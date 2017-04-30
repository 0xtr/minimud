#include "player_db_record.h"

struct player_db_record *get_player_db_struct(void)
{
	return (struct player_db_record *)calloc(sizeof(struct player_db_record), sizeof(struct player_db_record));
}
