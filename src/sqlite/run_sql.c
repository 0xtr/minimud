#include "run_sql.h"

int32_t run_sql(char *query, void *data, const int32_t type)
{
	printf("running %s\n", query);
	sqlite3 *db = NULL;
	void *func;
	uint8_t *sqlerr = NULL;

	if (type == DB_ROOM || type == DB_ROOM_COUNT)
		db = get_roomdb();
	if (type == DB_PLAYER || type == DB_PLAYER_COUNT)
		db = get_playerdb();
	
	if (type == DB_ROOM)
		func = room_callback;
	if (type == DB_PLAYER)
		func = player_callback;
	if (type == DB_PLAYER_COUNT || type == DB_ROOM_COUNT)
		func = count_callback;

	assert(db != NULL);

	if (sqlite3_exec(db, query, func, data, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(query);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(query);

	return EXIT_SUCCESS;
}
