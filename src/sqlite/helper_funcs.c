#include "helper_funcs.h"

static void handle_player_columns(char *azColName, char *arg1, struct PlayerDBRecord *player);
static void handle_map_columns(char *azColName, char *arg1, struct RoomRecord *map);

int room_callback (void *data, int argc, char **argv, char **azColName)
{
	struct RoomRecord *map_ref = data;

	increment_sqlite_rows_count();

	if (data == NULL)
		return EXIT_SUCCESS;

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
	const size_t len = strlen(azColName);

	if (azColName[0] == 'x') {
		player->x = atoi(arg1);
	} else if (azColName[0] == 'y') {
		player->y = atoi(arg1);
	} else if (azColName[0] == 'z') {
		player->z = atoi(arg1);
	} else if (strcmp(azColName, "hash") == 0) {
		player->hash = (uint8_t *)arg1;
	} else if (memcmp(azColName, "name", len) == 0) {
		player->name = (uint8_t *)arg1;
	} else if (memcmp(azColName, "salt", len) == 0) {
		player->salt = (uint8_t *)arg1;
	} else if (memcmp(azColName, "last_ip", len) == 0) {
		player->last_ip = (uint8_t *)arg1;
	}
}

static void handle_map_columns(char *azColName, char *arg1, struct RoomRecord *map)
{
	const size_t len = strlen(azColName);

	if (strcmp(azColName, "name") == 0) {
		memcpy(map->rname, arg1, len);
	} else if (strcmp(azColName, "desc") == 0) {
		memcpy(map->rdesc, arg1, len);
	} else if (strcmp(azColName, "north") == 0) {
		map->north = atoi(arg1);
	} else if (strcmp(azColName, "east") == 0) {
		map->east = atoi(arg1);
	} else if (strcmp(azColName, "south") == 0) {
		map->south = atoi(arg1);
	} else if (strcmp(azColName, "west") == 0) {
		map->west = atoi(arg1);
	} else if (strcmp(azColName, "up") == 0) {
		map->up = atoi(arg1);
	} else if (strcmp(azColName, "down") == 0) {
		map->down = atoi(arg1);
	} else if (strcmp(azColName, "northeast") == 0) {
		map->northeast = atoi(arg1);
	} else if (strcmp(azColName, "southeast") == 0) {
		map->southeast = atoi(arg1);
	} else if (strcmp(azColName, "southwest") == 0) {
		map->southwest = atoi(arg1);
	} else if (strcmp(azColName, "northwest") == 0) {
		map->northwest = atoi(arg1);
	} else if (strcmp(azColName, "owner") == 0) {
		memcpy(map->owner, arg1, len);
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
