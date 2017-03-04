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

struct Map *lookup_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket)
{
	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf("SELECT * FROM ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char)x, (char)y, (char)z);

	struct Map *map = get_room();

	if (sqlite3_exec(get_roomdb(), (char*)room, callback, map, (char**)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room lookup error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		free(map);
		return NULL;
	}
	sqlite3_free(room);

	if (socket == -1)
		return map;

	set_player_buffer_replace(socket, (uint8_t *)"> ");
	if (get_sqlite_rows_count() == 0) {
		set_player_buffer_append(socket, (uint8_t *)"NULL SPACE");
	} else {
		set_player_buffer_append(socket, map->rname);
	}

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	set_player_buffer_replace(socket, (uint8_t *)"[");
	// room x
	set_player_buffer_append(socket, (uint8_t *)(char*)&x);
	set_player_buffer_append(socket, (uint8_t *)"][");
	// room y
	set_player_buffer_append(socket, (uint8_t *)(char*)&y);
	set_player_buffer_append(socket, (uint8_t *)"][");
	// room z
	set_player_buffer_append(socket, (uint8_t *)(char*)&z);
	set_player_buffer_append(socket, (uint8_t *)"]");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	if (map == NULL) {
		set_player_buffer_replace(socket, (uint8_t *)"It is pitch black. You are likely to be eaten by a null character.");
	} else {
		set_player_buffer_replace(socket, map->rdesc);
	}
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return map;
}

int32_t lookup_room_exits(const int32_t socket, const int32_t xadj, const int32_t yadj, const int32_t zadj)
{
	struct Map *map = lookup_room(get_player_coord(X_COORD_REQUEST, socket), 
			               get_player_coord(Y_COORD_REQUEST, socket), 
			               get_player_coord(Z_COORD_REQUEST, socket), -1);

	if (map == NULL)
		return -2;

	if (!has_exit_for_dir(xadj, yadj, zadj, map)) {
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

int32_t has_exit_for_dir (const int32_t x, const int32_t y, const int32_t z, const struct Map *map) {
	if (is_vector_west(x, y) && !has_exit(map->west))
		return EXIT_FAILURE;
	if (is_vector_east(x, y) && !has_exit(map->east))
		return EXIT_FAILURE;
	if (is_vector_north(x, y) && !has_exit(map->north))
		return EXIT_FAILURE;
	if (is_vector_south(x, y) && !has_exit(map->south))
		return EXIT_FAILURE;
	if (is_vector_up(z) && !has_exit(map->up))
		return EXIT_FAILURE;
	if (is_vector_down(z) && !has_exit(map->down))
		return EXIT_FAILURE;
	if (is_vector_northeast(x, y) && !has_exit(map->northeast))
		return EXIT_FAILURE;
	if (is_vector_southeast(x, y) && !has_exit(map->southeast))
		return EXIT_FAILURE;
	if (is_vector_southwest(x, y) && !has_exit(map->southwest))
		return EXIT_FAILURE;
	if (is_vector_northwest(x, y) && !has_exit(map->northwest))
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}

int32_t lookup_room_name_from_coords(const int32_t socket, const int32_t x, const int32_t y, const int32_t z)
{
	struct Map *map = lookup_room(x, y, z, -1);

	if (map != NULL) {
		// get_room_details(map);
		set_player_buffer_replace(socket, map->rname);
		free_room(map);
	} else {
		set_player_buffer_replace(socket, (uint8_t *)"an unknown location");
	}

	return EXIT_SUCCESS;
}

int32_t compare_room_owner(const int32_t socket, const int32_t x, const int32_t y, const int32_t z)
{
	struct Map *map = lookup_room (x, y, z, -1);

	if (map == NULL)
		return -1;

	if (strcmp((char*)get_player_name(socket), (char*)map->owner) != 0) {
		free_room(map);
		return EXIT_FAILURE;
	}

	free_room(map);

	return EXIT_SUCCESS;
}
