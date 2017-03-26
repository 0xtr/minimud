#include "lookup_player.h"

struct PlayerDBRecord *lookup_player(const uint8_t *name)
{
	uint8_t *sqlerr = NULL;
	// check for players with the same name 
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", name);

	struct PlayerDBRecord *player = get_player_db_struct();

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, player_callback, player, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 error in lookup_player:\n%s\n", sqlite3_errmsg(get_playerdb()));
		free(player);
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		return NULL;
	}

	sqlite3_free(pcheck);

	if (strlen((char *)player->name) == 0) {
		free(player);
		return NULL;
	}

	return player;
}
