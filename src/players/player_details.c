#include "player_details.h"

struct coordinates get_player_coords(const int32_t socket)
{
	struct PlayerDBRecord *player = get_player_db_struct();
	struct coordinates coords;
	coords.x = coords.y = coords.z = -1;

	run_sql(sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", get_player_name(socket)), player, DB_PLAYER);

	coords.x = player->x;
	coords.y = player->y;
	coords.z = player->z;

	free(player);

	return coords;
}

int32_t get_next_player_num(void)
{
	run_sql(sqlite3_mprintf("SELECT id FROM PLAYERS;"), 0, DB_PLAYER);
	reset_sqlite_rows_count();

	return get_sqlite_rows_count();
}
