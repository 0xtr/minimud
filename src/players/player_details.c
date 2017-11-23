#include "player_details.h"

struct coordinates get_player_coords(struct player_live_record *player)
{
	struct player_db_record *player_db = get_player_db_struct();
	struct coordinates coords;
	coords.x = coords.y = coords.z = -1;

	run_sql(sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name = %Q;", player->name), player_db, DB_PLAYER);

	struct room_atom *room = lookup_room_by_id(player_db->loc_id);
	coords = room->coords;

	free(player_db);
	free(room);

	return coords;
}

int32_t get_player_loc_id(struct player_live_record *player)
{
	struct player_db_record *player_db = get_player_db_struct();

	run_sql(sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name = %Q;", player->name), player_db, DB_PLAYER);

	const int32_t loc_id = player_db->loc_id;

	free(player_db);

	return loc_id;
}

int32_t get_next_player_num(void)
{
	struct query_matches *qmatches = init_query();

	assert(run_sql("SELECT id FROM PLAYERS;", qmatches, 
				DB_PLAYER_COUNT) == EXIT_SUCCESS);
	const int32_t max_id = qmatches->matches;

	free(qmatches);

	return max_id;
}
