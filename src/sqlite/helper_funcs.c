#include "helper_funcs.h"

static void handle_player_columns(char *azColName, char *arg1);
static void handle_map_columns(char *azColName, char *arg1, struct Map *map);

int callback (void *map, int argc, char **argv, char **azColName)
{
	struct Map *map_ref = map;
	increment_sqlite_rows_count();

	for (size_t i = 0; i < (size_t)argc; ++i) {
		if (map != NULL) {
			handle_map_columns(azColName[i], argv[i], map_ref);
		} else {
			handle_player_columns(azColName[i], argv[i]);
		}
	}

	// return 0 or callback will request an abort
	return EXIT_SUCCESS;
}

static void handle_player_columns(char *azColName, char *arg1)
{
	printf("%s\n", arg1);
	if (azColName[0] == 'x') {
		//player_tmp.x = atoi(argv[i]);
	} else if (azColName[0] == 'y') {
		//player_tmp.y = atoi(argv[i]);
	} else if (azColName[0] == 'z') {
		//player_tmp.z = atoi(argv[i]);
	} else if (strcmp(azColName, "hash") == 0) {
		//memcpy(player_tmp.hash, argv[i], HASHSPACE);
	}
}

static void handle_map_columns(char *azColName, char *arg1, struct Map *map)
{
	if (strcmp(azColName, "rname") == 0) {
		memcpy(map->rname, arg1, NAMES_MAX);
	} else if (strcmp(azColName, "rdesc") == 0) {
		memcpy(map->rdesc, arg1, BUFFER_LENGTH);
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
		memcpy(map->owner, arg1, NAMES_MAX);
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
