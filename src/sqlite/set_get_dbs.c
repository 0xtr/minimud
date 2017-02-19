#include "set_get_dbs.h"

static sqlite3 *roomdb;
static sqlite3 *playerdb;

sqlite3 *get_roomdb(void)
{
	assert(roomdb != NULL);
	return roomdb;
}

void set_roomdb(sqlite3 *db)
{
	roomdb = db;
}

sqlite3 *get_playerdb(void)
{
	assert(playerdb != NULL);
	return playerdb;
}

void set_playerdb(sqlite3 *db)
{
	playerdb = db;
}
