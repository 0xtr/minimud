#include "lookup_player.h"

struct player_db_record *lookup_player(const uint8_t *name)
{
	struct player_db_record *player = get_player_db_struct();

	run_sql(sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", name), player, DB_PLAYER);

	if (strlen((char *)player->name) == 0) {
		free(player);
		return NULL;
	}

	return player;
}
