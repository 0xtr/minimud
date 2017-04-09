#include "helper_funcs.h"

static void handle_player_columns(char *azColName, char *arg1, struct PlayerDBRecord *player);
static void handle_map_columns(char *azColName, char *arg1, struct RoomRecord *map);

int room_callback (void *data, int argc, char **argv, char **azColName)
{
	struct RoomRecord *map_ref = data;

	increment_sqlite_rows_count();

	if (data == NULL)
		return EXIT_SUCCESS;

	map_ref->found = true;

	for (size_t i = 0; i < (size_t)argc; ++i)
		handle_map_columns(azColName[i], argv[i], map_ref);

	// return 0 or callback will request an abort
	return EXIT_SUCCESS;
}

int player_callback (void *data, int argc, char **argv, char **azColName)
{
	struct PlayerDBRecord *player_ref = data;

	increment_sqlite_rows_count();

	if (data == NULL)
		return EXIT_SUCCESS;

	for (size_t i = 0; i < (size_t)argc; ++i)
		handle_player_columns(azColName[i], argv[i], player_ref);

	// return 0 or callback will request an abort
	return EXIT_SUCCESS;
}


static void handle_player_columns(char *azColName, char *arg1, struct PlayerDBRecord *player)
{
	const size_t arg_len = strlen(arg1);
	const size_t col_len = strlen(azColName);

	if (azColName[0] == 'x') {
		player->x = atoi(arg1);
	} else if (azColName[0] == 'y') {
		player->y = atoi(arg1);
	} else if (azColName[0] == 'z') {
		player->z = atoi(arg1);
	} else if (memcmp(azColName, "hash", col_len) == 0) {
		memcpy(player->hash, arg1, arg_len);
	} else if (memcmp(azColName, "name", col_len) == 0) {
		memcpy(player->name, arg1, arg_len);
	} else if (memcmp(azColName, "salt", col_len) == 0) {
		memcpy(player->salt, arg1, arg_len);
	} else if (memcmp(azColName, "last_ip", col_len) == 0) {
		memcpy(player->last_ip, arg1, arg_len);
	}
}

static void handle_map_columns(char *azColName, char *arg1, struct RoomRecord *map)
{
	const size_t arg_len = strlen(arg1);
	const size_t col_len = strlen(azColName);

	if (memcmp(azColName, "name", col_len) == 0) {
		memcpy(map->rname, arg1, arg_len);
	} else if (memcmp(azColName, "desc", col_len) == 0) {
		memcpy(map->rdesc, arg1, arg_len);
	} else if (memcmp(azColName, "north", col_len) == 0) {
		map->north = atoi(arg1);
	} else if (memcmp(azColName, "east", col_len) == 0) {
		map->east = atoi(arg1);
	} else if (memcmp(azColName, "south", col_len) == 0) {
		map->south = atoi(arg1);
	} else if (memcmp(azColName, "west", col_len) == 0) {
		map->west = atoi(arg1);
	} else if (memcmp(azColName, "up", col_len) == 0) {
		map->up = atoi(arg1);
	} else if (memcmp(azColName, "down", col_len) == 0) {
		map->down = atoi(arg1);
	} else if (memcmp(azColName, "northeast", col_len) == 0) {
		map->northeast = atoi(arg1);
	} else if (memcmp(azColName, "southeast", col_len) == 0) {
		map->southeast = atoi(arg1);
	} else if (memcmp(azColName, "southwest", col_len) == 0) {
		map->southwest = atoi(arg1);
	} else if (memcmp(azColName, "northwest", col_len) == 0) {
		map->northwest = atoi(arg1);
	} else if (memcmp(azColName, "owner", col_len) == 0) {
		memcpy(map->owner, arg1, arg_len);
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
