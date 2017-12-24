#include "sqlite_init_dbs.h"

static void open_playerdb(void);
static void open_roomdb(void);
static void open_objdb(void);
static void insert_base_room(void);

#define open_or_create_db(db, loc) \
	if (access((char*)loc, F_OK) == -1)\
		tables_needed = true;\
	assert(sqlite3_open_v2(loc, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);\

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
	open_objdb();

	insert_base_room();

	return EXIT_SUCCESS;
}

static void insert_base_room(void)
{
	struct coordinates coords = {0};
	struct room_db_record *room = lookup_room(coords);

	const int32_t id = room->id;

	free(room);

	if (id > 0)
		goto success;

	// check that we have at least the origin room
	struct room_blueprint rconfig;
	rconfig.name = (uint8_t*)"The Core of the World";
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.coords.x = 0;
	rconfig.coords.y = 0;
	rconfig.coords.z = 0;
	rconfig.owner = (uint8_t *)"system";
	rconfig.flags = (uint8_t *)"none";

	struct room_db_record *result = insert_room(rconfig);
	assert(result->id == 1);
	free(result);

	success:;
}

static void open_playerdb(void)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	open_or_create_db(db, SQLITE_PLAYERDB_LOC);
	set_playerdb(db);

	if (tables_needed)
		assert(run_sql(sqlite3_mprintf(
			"CREATE TABLE PLAYERS (id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"name TEXT, hash TEXT, salt TEXT, last_ip TEXT,"
			"loc_id INT)"), 0, DB_PLAYER) == EXIT_SUCCESS);
}

static void open_objdb(void)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	open_or_create_db(db, SQLITE_OBJDB_LOC);
	set_objdb(db);

	if (tables_needed)
		assert(run_sql(sqlite3_mprintf(
			"CREATE TABLE OBJECTS (id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"obj_name TEXT, obj_keywords TEXT, obj_desc TEXT, obj_createdby TEXT,"
			"obj_location INT, obj_playerid INT)"), 0, DB_OBJECT) == EXIT_SUCCESS);
}

static void open_roomdb(void)
{
	_Bool tables_needed = false;
	sqlite3 *db;

	open_or_create_db(db, SQLITE_ROOMDB_LOC);
	set_roomdb(db);

	if (tables_needed)
		assert(run_sql(sqlite3_mprintf(
			"CREATE TABLE ROOMS (id INTEGER PRIMARY KEY AUTOINCREMENT," 
			"name TEXT, desc TEXT, "
			"x INT, y INT, z  INT,"
			"north INT, east INT, south INT,"
			"west  INT, up   INT, down  INT,"
			"northeast INT, southeast INT, southwest INT," "northwest INT,"
			"owner TEXT, last_modified_by TEXT,"
			"flags TEXT)"), 0, DB_ROOM) == 0);
}
