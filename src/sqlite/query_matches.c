#include "query_matches.h"

struct query_matches *players_in_room(const int32_t r_id)
{
	uint8_t r_id_str[2 * sizeof(r_id)] = {0};
	snprintf((char *)r_id_str, sizeof(r_id), "%d", r_id);

	struct query_matches *matches = init_query();
	int32_t rv = run_sql(
			sqlite3_mprintf("SELECT id FROM PLAYERS WHERE loc_id = %Q",
			r_id_str), matches, DB_PLAYER_COUNT);

	assert(rv == 0);

	return matches;
}

struct query_matches *init_query(void)
{
	return (struct query_matches *)calloc(sizeof(struct query_matches), sizeof(struct query_matches));
}

void add_query_match(struct query_matches *qmatches, const char *id)
{
	++qmatches->matches;
	qmatches->ids = realloc(qmatches->ids, sizeof(qmatches->ids) + sizeof(int32_t));
	qmatches->ids[(qmatches->matches-1)] = atoi(id);
	for (size_t i = 0; i < qmatches->matches; ++i) {
		printf("%d ", qmatches->ids[i]);
	}
	printf("\n");
}
