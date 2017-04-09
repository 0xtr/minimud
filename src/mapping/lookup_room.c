#include "lookup_room.h"

static _Bool is_vector_west(const int32_t x, const int32_t y);
static _Bool is_vector_east(const int32_t x, const int32_t y);
static _Bool is_vector_north(const int32_t x, const int32_t y);
static _Bool is_vector_south(const int32_t x, const int32_t y);
static _Bool is_vector_northwest(const int32_t x, const int32_t y);
static _Bool is_vector_down(const int32_t z);
static _Bool is_vector_up(const int32_t z);
static _Bool is_vector_northwest(const int32_t x, const int32_t y);
static _Bool is_vector_northeast(const int32_t x, const int32_t y);
static _Bool is_vector_southeast(const int32_t x, const int32_t y);
static _Bool is_vector_southwest(const int32_t x, const int32_t y);
static _Bool has_exit(const int32_t exit_value);

struct RoomRecord *lookup_room(const struct Coordinates coords)
{
	uint8_t param_x[sizeof(coords.x)] = {0};
	uint8_t param_y[sizeof(coords.y)] = {0};
	uint8_t param_z[sizeof(coords.z)] = {0};
	snprintf((char *)param_x, sizeof(coords.x), "%d", coords.x);
	snprintf((char *)param_y, sizeof(coords.y), "%d", coords.y);
	snprintf((char *)param_z, sizeof(coords.z), "%d", coords.z);

	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf(
			"SELECT * FROM ROOMS WHERE x LIKE %Q AND y LIKE %Q AND z LIKE %Q;", 
			param_x, param_y, param_z);
	struct RoomRecord *map = get_room();

	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, map, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		free(map);
		return NULL;
	}

	sqlite3_free(room);

	return map;
}

int32_t lookup_room_exits(const int32_t socket, const struct Coordinates coords)
{
	struct Coordinates original_coords = get_player_coords(socket);
	struct RoomRecord *map = lookup_room(coords);

	if (map == NULL)
		return -2;

	if (!has_exit_for_dir(original_coords, map)) {
		free_room(map);
		return -1;
	}

	free_room(map);

	return EXIT_SUCCESS;
}

static _Bool is_vector_west(const int32_t x, const int32_t y)
{
	return x == -1 && y == 0;
}

static _Bool is_vector_east(const int32_t x, const int32_t y)
{
	return x == 1 && y == 0;
}

static _Bool is_vector_north(const int32_t x, const int32_t y)
{
	return x == 0 && y == 1;
}

static _Bool is_vector_south(const int32_t x, const int32_t y)
{
	return x == 0 && y == -1;
}

static _Bool is_vector_up(const int32_t z)
{
	return z == 1;
}

static _Bool is_vector_down(const int32_t z)
{
	return z == -1;
}

static _Bool is_vector_northwest(const int32_t x, const int32_t y)
{
	return x == -1 && y == 1;
}

static _Bool is_vector_southwest(const int32_t x, const int32_t y)
{
	return x == -1 && y == -1;
}

static _Bool is_vector_northeast(const int32_t x, const int32_t y)
{
	return x == 1 && y == 1;
}

static _Bool is_vector_southeast(const int32_t x, const int32_t y)
{
	return x == 1 && y == -1;
}

static _Bool has_exit(const int32_t exit_value)
{
	return exit_value != 0;
}

int32_t has_exit_for_dir (const struct Coordinates coords, const struct RoomRecord *map) {
	if (is_vector_west(coords.x, coords.y) && !has_exit(map->west))
		return EXIT_FAILURE;
	if (is_vector_east(coords.x, coords.y) && !has_exit(map->east))
		return EXIT_FAILURE;
	if (is_vector_north(coords.x, coords.y) && !has_exit(map->north))
		return EXIT_FAILURE;
	if (is_vector_south(coords.x, coords.y) && !has_exit(map->south))
		return EXIT_FAILURE;
	if (is_vector_up(coords.z) && !has_exit(map->up))
		return EXIT_FAILURE;
	if (is_vector_down(coords.z) && !has_exit(map->down))
		return EXIT_FAILURE;
	if (is_vector_northeast(coords.x, coords.y) && !has_exit(map->northeast))
		return EXIT_FAILURE;
	if (is_vector_southeast(coords.x, coords.y) && !has_exit(map->southeast))
		return EXIT_FAILURE;
	if (is_vector_southwest(coords.x, coords.y) && !has_exit(map->southwest))
		return EXIT_FAILURE;
	if (is_vector_northwest(coords.x, coords.y) && !has_exit(map->northwest))
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

int32_t lookup_room_name_from_coords(const int32_t socket, const struct Coordinates coords)
{
	struct RoomRecord *map = lookup_room(coords);

	if (map != NULL) {
		// get_room_details(map);
		set_player_buffer_replace(socket, map->rname);
		free_room(map);
	} else {
		set_player_buffer_replace(socket, (uint8_t *)"an unknown location");
	}

	return EXIT_SUCCESS;
}

int32_t compare_room_owner(const int32_t socket, const struct Coordinates coords)
{
	struct RoomRecord *map = lookup_room(coords);

	if (map == NULL)
		return -1;

	if (strcmp((char*)get_player_name(socket), (char*)map->owner) != 0) {
		free_room(map);
		return EXIT_FAILURE;
	}

	free_room(map);

	return EXIT_SUCCESS;
}
