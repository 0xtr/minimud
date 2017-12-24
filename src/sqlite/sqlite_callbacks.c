#include "sqlite_callbacks.h"

static void handle_player_columns(char *azColName, char *arg1, struct player_db_record *player);
static void handle_map_columns(char *azColName, char *arg1, struct room_db_record *map);

int room_callback (void *data, int argc, char **argv, char **azColName)
{
	struct room_db_record *map_ref = data;

	increment_sqlite_rows_count();

	if (data == NULL)
		return EXIT_SUCCESS;

	memset(map_ref->exits, -1, sizeof(map_ref));
	map_ref->found = true;

	for (size_t i = 0; i < (size_t)argc; ++i)
		handle_map_columns(azColName[i], argv[i], map_ref);

	return EXIT_SUCCESS;
}

int count_callback (void *data, int argc, char **argv, char **azColName)
{
	struct query_matches *qmatches = data;

	for (size_t i = 0; i < (size_t)argc; ++i) {
		if (memcmp(azColName[i], "id", strlen(azColName[i])) != 0)
			continue;

		add_query_match(qmatches, argv[i]);
		break;
	}

	return EXIT_SUCCESS;
}

int player_callback (void *data, int argc, char **argv, char **azColName)
{
	struct player_db_record *player_ref = data;

	increment_sqlite_rows_count();

	if (data == NULL)
		return EXIT_SUCCESS;

	for (size_t i = 0; i < (size_t)argc; ++i)
		handle_player_columns(azColName[i], argv[i], player_ref);

	return EXIT_SUCCESS;
}


static void handle_player_columns(char *azColName, char *arg1, struct player_db_record *player)
{
	const size_t arg_len = strlen(arg1);
	const size_t col_len = strlen(azColName);

	if (memcmp(azColName, "loc_id", col_len) == 0) {
		player->loc_id = atoi(arg1);
	} else if (memcmp(azColName, "hash", col_len) == 0) {
		memcpy(player->hash, arg1, arg_len);
	} else if (memcmp(azColName, "name", col_len) == 0) {
		memcpy(player->name, arg1, arg_len);
	} else if (memcmp(azColName, "salt", col_len) == 0) {
		memcpy(player->salt, arg1, arg_len);
	} else if (memcmp(azColName, "last_ip", col_len) == 0) {
		memcpy(player->last_ip, arg1, arg_len);
	} else if (memcmp(azColName, "id", col_len) == 0) {
		player->id = atoi(arg1);
	}
}

static void handle_map_columns(char *azColName, char *arg1, struct room_db_record *map)
{
	const size_t arg_len = (arg1 != NULL) ? strlen(arg1) : 0;
	const size_t col_len = strlen(azColName);

	if (memcmp(azColName, "name", col_len) == 0) {
		memcpy(map->rname, arg1, arg_len);
	} else if (memcmp(azColName, "desc", col_len) == 0) {
		memcpy(map->rdesc, arg1, arg_len);
	} else if (memcmp(azColName, "north", col_len) == 0) {
		map->exits[NORTH_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "east", col_len) == 0) {
		map->exits[EAST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "south", col_len) == 0) {
		map->exits[SOUTH_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "west", col_len) == 0) {
		map->exits[WEST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "up", col_len) == 0) {
		map->exits[UP_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "down", col_len) == 0) {
		map->exits[DOWN_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "northeast", col_len) == 0) {
		map->exits[NORTHEAST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "southeast", col_len) == 0) {
		map->exits[SOUTHEAST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "southwest", col_len) == 0) {
		map->exits[SOUTHWEST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "northwest", col_len) == 0) {
		map->exits[NORTHWEST_EXIT] = atoi(arg1);
	} else if (memcmp(azColName, "owner", col_len) == 0) {
		memcpy(map->owner, arg1, arg_len);
	} else if (memcmp(azColName, "id", col_len) == 0) {
		map->id = atoi(arg1);
	} else if (memcmp(azColName, "x", col_len) == 0) {
		map->coords.x = atoi(arg1);
	} else if (memcmp(azColName, "y", col_len) == 0) {
		map->coords.y = atoi(arg1);
	} else if (memcmp(azColName, "z", col_len) == 0) {
		map->coords.z = atoi(arg1);
	}
}

static int rows_count;
int32_t get_sqlite_rows_count(void)
{
	return rows_count;
}
void increment_sqlite_rows_count(void)
{
	++rows_count;
}
void set_sqlite_rows_count(const int32_t newval)
{
	rows_count = newval;
}
void reset_sqlite_rows_count(void)
{
	rows_count = 0;
}
