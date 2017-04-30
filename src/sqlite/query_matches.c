#include "query_matches.h"

struct query_matches *players_in_room(const struct coordinates coords)
{
	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	struct query_matches *matches = init_query();
	int32_t rv = run_sql(
			sqlite3_mprintf("SELECT id FROM PLAYERS WHERE x LIKE %Q AND y LIKE %Q AND z LIKE %Q",
			param_x, param_y, param_z), matches, DB_PLAYER_COUNT);

	assert(rv == 0);

	return matches;
}

struct query_matches *init_query(void)
{
	return (struct query_matches *)calloc(sizeof(struct query_matches), sizeof(struct query_matches));
}

void add_query_match(struct query_matches *qmatches, const char *id)
{
	qmatches->ids = realloc(qmatches->ids, sizeof(qmatches->ids) + sizeof(int32_t));
	qmatches->ids[sizeof(qmatches->ids)/sizeof(qmatches->ids[0])] = atoi(id);
}
