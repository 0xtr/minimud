#include "player_details.h"

struct Coordinates get_player_coords(const int32_t socket)
{
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", get_player_name(socket));
	uint8_t *sqlerr = NULL;

	struct PlayerDBRecord *player = get_player_db_struct();
	struct Coordinates coords;
	coords.x = coords.y = coords.z = -1;

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, player_callback, player, (char **)sqlerr) != SQLITE_OK) {
	#ifdef DEBUG
		fprintf(stdout, "SQLITE3 error in get_player_coord:\n%s\n", sqlite3_errmsg(get_playerdb()));
	#endif
		free(player);
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		return coords;
	}

	sqlite3_free(pcheck);

	coords.x = player->x;
	coords.y = player->y;
	coords.z = player->z;

	free(player);

	return coords;
}

int32_t get_next_player_num(void)
{
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS;");
	uint8_t *sqlerr = NULL;
	reset_sqlite_rows_count();

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, player_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		#ifdef DEBUG
			fprintf(stdout, "SQLITE3 error in get_next_player_num:\n%s\n", sqlite3_errmsg(get_playerdb()));
		#endif
		return EXIT_FAILURE;
	}

	sqlite3_free(pcheck);
	return get_sqlite_rows_count();
}
