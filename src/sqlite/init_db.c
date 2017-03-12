#include "init_db.h"

static void open_playerdb(void);
static void open_roomdb(void);

int32_t init_dbs(void)
{
	DIR* dir = opendir("dbs");
	if (dir) {
		closedir(dir);
	} else if (errno == ENOENT) {
		mkdir("./dbs", 0777);
	} else {
		perror("database dir error");
		return EXIT_FAILURE;
	}

	open_playerdb();
	open_roomdb();

	// check that we have at least the origin room
	struct NewRoom rconfig;
	rconfig.name = (uint8_t*)"The Core of the World";
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.x = 0;
	rconfig.y = 0;
	rconfig.z = 0;
	rconfig.owner = (uint8_t *)"system";
	rconfig.flags = (uint8_t *)"none";
	assert(insert_room(rconfig) == 0);

	return EXIT_SUCCESS;
}

static void open_playerdb(void)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	if (access((char*)SQLITE_PLAYERDB_LOC, F_OK) == -1)
		tables_needed = true;

	assert(sqlite3_open_v2(SQLITE_PLAYERDB_LOC, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);
	set_playerdb(db);

	if (tables_needed)
		assert(sqlite3_exec(get_playerdb(), 
			"CREATE TABLE PLAYERS (pnum INTEGER PRIMARY KEY," 
			"name TEXT, hash TEXT," "salt TEXT," "last_ip TEXT,"
			"x INT, y INT, z INT)", callback, 0, NULL) == SQLITE_OK); 
}

static void open_roomdb(void)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	if (access((char*)SQLITE_ROOMDB_LOC, F_OK) == -1)
		tables_needed = true;

	assert(sqlite3_open_v2(SQLITE_ROOMDB_LOC, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);
	set_roomdb(db);

	if (tables_needed)
		assert(sqlite3_exec(get_roomdb(), 
			"CREATE TABLE ROOMS (id INTEGER PRIMARY KEY," 
			"rname TEXT, rdesc TEXT," "xloc  INTEGER, yloc INT, zloc  INT,"
			"north INTEGER, east INT, south INT,"
			"west  INTEGER, up   INT, down  INT,"
			"northeast INTEGER, southeast INT, southwest INT,"
			"northwest INTEGER," "owner TEXT," "last_modified_by TEXT,"
			"flags TEXT)", callback, 0, NULL) == SQLITE_OK); 
}
