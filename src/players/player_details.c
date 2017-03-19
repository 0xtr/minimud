#include "player_details.h"

int32_t get_player_coord(const int32_t coord_type, const int32_t socket)
{
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", get_player_name(socket));
	uint8_t *sqlerr = NULL;

	struct PlayerDBRecord *player = get_player_db_struct();

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, player_callback, player, (char **)sqlerr) != SQLITE_OK) {
	#ifdef DEBUG
		fprintf(stdout, "SQLITE3 error in get_player_coord:\n%s\n", sqlite3_errmsg(get_playerdb()));
	#endif
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(pcheck);

	int32_t val = 0;
	if (coord_type == X_COORD_REQUEST) {
		val = player->x;
	} else if (coord_type == Y_COORD_REQUEST) {
		val = player->y;
	} else if (coord_type == Z_COORD_REQUEST) {
		val = player->z;
	} else {
		val = -1;
	}

	free(player);

	return val;
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
