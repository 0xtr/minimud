#include "run_sql.h"

int32_t run_sql(char *query, void *data, const int32_t type)
{
	printf("running %s\n", query);
	sqlite3 *db = NULL;
	void *func;
	uint8_t *sqlerr = NULL;

	if (type == DB_ROOM) {
		db = get_roomdb();
		func = room_callback;
	}
	if (type == DB_PLAYER) {
		db = get_playerdb();
		func = player_callback;
	}

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
