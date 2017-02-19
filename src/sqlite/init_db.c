#include "init_db.h"

static const uint8_t *BASE_ROOM_NAME = (uint8_t*)"The Core of the World";
static const uint8_t *BASE_ROOM_DESC = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character." ;

int32_t init_db(const int32_t DB_TYPE)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	if (access((char*)SQLITE_ROOMDB_LOC, F_OK) == -1 && DB_TYPE == ROOM_DB_TYPE)
		tables_needed = true;
	if (access((char*)SQLITE_PLAYERDB_LOC, F_OK) == -1 && DB_TYPE == PLAYER_DB_TYPE)
		tables_needed = true;

	assert(sqlite3_open_v2((DB_TYPE == ROOM_DB_TYPE) 
		?  SQLITE_ROOMDB_LOC 
		: SQLITE_PLAYERDB_LOC, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL) == 0);
	(DB_TYPE == ROOM_DB_TYPE) ? set_roomdb(db) : set_playerdb(db);

	if (tables_needed == true) {
		assert(sqlite3_exec(get_playerdb(), 
		        "CREATE TABLE PLAYERS (pnum INTEGER PRIMARY KEY," 
		        "pname  TEXT, hash TEXT," "salt   TEXT," "last_ip TEXT,"
		        "x INT, y INT, z INT)", callback, 0, NULL) == SQLITE_OK); 
		assert(sqlite3_exec(get_roomdb(), 
		        "CREATE TABLE ROOMS (id INTEGER PRIMARY KEY," 
		        "rname TEXT, rdesc TEXT," "xloc  INTEGER, yloc INT, zloc  INT,"
		        "north INTEGER, east INT, south INT,"
		        "west  INTEGER, up   INT, down  INT,"
		        "northeast INTEGER, southeast INT, southwest INT,"
		        "northwest INTEGER," "owner TEXT," "last_modified_by TEXT,"
		        "flags TEXT)", callback, 0, NULL) == SQLITE_OK); 
	}

	// check that we have at least the origin room
	assert(insert_room(BASE_ROOM_NAME, 0, 0, 0, BASE_ROOM_DESC, (uint8_t*)"admin", (uint8_t*)"none"));
	return EXIT_SUCCESS;
}
