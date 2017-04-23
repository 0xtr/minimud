#include "lookup_player.h"

struct PlayerDBRecord *lookup_player(const uint8_t *name)
{
	struct PlayerDBRecord *player = get_player_db_struct();

	// check for players with the same name 
	run_sql(sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", name), player, DB_PLAYER);

	if (strlen((char *)player->name) == 0) {
		free(player);
		return NULL;
	}

	return player;
}
