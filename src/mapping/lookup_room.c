#include "lookup_room.h"

struct room_atom *lookup_room(const struct coordinates coords)
{
	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	struct room_atom *map = get_room();

	int32_t rv = run_sql(sqlite3_mprintf(
			"SELECT * FROM ROOMS WHERE x LIKE %Q AND y LIKE %Q AND z LIKE %Q;", 
			param_x, param_y, param_z), map, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return NULL;

	return map;
}

struct room_atom *lookup_room_by_id(const int32_t id)
{
	uint8_t idstr[sizeof(id)] = {0};
	snprintf((char *)idstr, sizeof(id), "%d", id);

	struct room_atom *map = get_room();

	int32_t rv = run_sql(sqlite3_mprintf(
			"SELECT * FROM ROOMS WHERE id LIKE %Q;", idstr), map, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return NULL;

	return map;
}

int32_t lookup_room_exits(const struct coordinates origin, const struct coordinates dest)
{
	int32_t rv = 0;
	struct room_atom *origin_room = lookup_room(origin);
	struct room_atom *dest_room = lookup_room(dest);

	if (dest_room == NULL) {
		rv = -2;
		goto end;
	}

	if (has_exit_for_dir(origin_room, dest_room) == EXIT_FAILURE)
		rv = -1;

	end: 

	free(origin_room);
	free(dest_room);

	return rv;
}

int32_t has_exit_for_dir(struct room_atom *origin, struct room_atom *dest)
{
	for (size_t i = 0; i < 10; ++i) {
		if (dest->exits[i] == -1)
			continue;

		if (memcmp(&origin->id, &dest->exits[i], sizeof(int32_t)) == 0)
			return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int32_t lookup_room_name_from_coords(const int32_t socket, const struct coordinates coords)
{
	struct room_atom *map = lookup_room(coords);

	if (map != NULL) {
		// get_room_details(map);
		set_player_buffer_replace(socket, map->rname);
		free(map);
	} else {
		set_player_buffer_replace(socket, "NULL SPACE");
	}

	return EXIT_SUCCESS;
}

int32_t compare_room_owner(const int32_t socket, const struct coordinates coords)
{
	int32_t rv = EXIT_SUCCESS;
	struct room_atom *map = lookup_room(coords);

	if (strcmp((char*)get_player_name(socket), (char*)map->owner) != 0)
		rv = EXIT_FAILURE;

	free(map);

	return rv;
}
