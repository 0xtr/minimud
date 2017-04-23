#include "room_adjustments.h"

#define player_is_not_owner strcmp((char *)map->owner, (char *)get_player_name(socket)) 

int32_t remove_players_from_room(const struct coordinates coords)
{
	// get_players_in_room(x, y, z);
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct coordinates this_player = get_player_coords(i);
		if (this_player.x == coords.x &&
			this_player.y == coords.y &&
			this_player.z == coords.z) {
			print_to_player(i, PRINT_REMOVED_FROM_ROOM);
			this_player.x = this_player.y = this_player.z = -1;
			adjust_player_location(i, this_player);
			print_to_player(i, SHOWROOM);
		}
	}
	return EXIT_SUCCESS;
}

int32_t adjust_room_name(const int32_t socket)
{
	struct coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	int32_t rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET name = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_player_store(socket), get_player_name(socket), param_x, param_y, param_z), 0, DB_ROOM);

	return rv;
}

int32_t adjust_room_desc(const int32_t socket)
{
	struct coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	int32_t rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET desc = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_player_store(socket), get_player_name(socket), param_x, param_y, param_z), 0, DB_ROOM);

	return rv;
}

int32_t adjust_room_flag(const int32_t socket)
{
	struct coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	// TODO: this
	int32_t rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET flags = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_player_name(socket), param_x, param_y, param_z), 0, DB_ROOM);

	return rv;
}

int32_t adjust_room_exit(const int32_t dir, struct room_atom *existing, struct room_atom *newroom)
{
	convert_coords_into_string_params(existing->coords.x, existing->coords.y, existing->coords.z);

	uint8_t id[sizeof(int32_t)] = {0};
	snprintf((char *)id, sizeof(newroom->id), "%d", newroom->id);

	int32_t rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET %Q = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_movement_str(dir), id, param_x, param_y, param_z), 0, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return rv;

	snprintf((char *)param_x, sizeof(newroom->coords.x), "%d", newroom->coords.x);
	snprintf((char *)param_y, sizeof(newroom->coords.y), "%d", newroom->coords.y);
	snprintf((char *)param_z, sizeof(newroom->coords.z), "%d", newroom->coords.z);
	snprintf((char *)id, sizeof(existing->id), "%d", existing->id);

	rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET %Q = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_opposite_str(dir), id, param_x, param_y, param_z), 0, DB_ROOM);

	return rv;
}
