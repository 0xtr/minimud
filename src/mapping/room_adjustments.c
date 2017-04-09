#include "room_adjustments.h"

#define player_is_not_owner strcmp((char *)map->owner, (char *)get_player_name(socket)) 
#define convert_coords_into_string_params \
	uint8_t param_x[2 * sizeof(coords.x)] = {0};\
	uint8_t param_y[2 * sizeof(coords.y)] = {0};\
	uint8_t param_z[2 * sizeof(coords.z)] = {0};\
	snprintf((char *)param_x, sizeof(coords.x), "%d", coords.x);\
	snprintf((char *)param_y, sizeof(coords.y), "%d", coords.y);\
	snprintf((char *)param_z, sizeof(coords.z), "%d", coords.z);

int32_t remove_players_from_room(const struct Coordinates coords)
{
	// get_players_in_room(x, y, z);
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct Coordinates this_player = get_player_coords(i);
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
	struct Coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params;

	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET name = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
			get_player_store(socket), get_player_name(socket), param_x, param_y, param_z);

	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	return EXIT_SUCCESS;
}

int32_t adjust_room_desc(const int32_t socket)
{
	struct Coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params;

	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET desc = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
			get_player_store(socket), get_player_name(socket), param_x, param_y, param_z);

	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	return EXIT_SUCCESS;
}

// TODO: split down
int32_t adjust_room_flag(const int32_t socket)
{
	struct Coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	convert_coords_into_string_params;

	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET flags = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
			get_player_name(socket), param_x, param_y, param_z);

	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	return EXIT_SUCCESS;
}

int32_t adjust_room_exit(const int32_t socket, struct Coordinates destroom)
{
	struct Coordinates coords = get_player_coords(socket);

	if (compare_room_owner(socket, coords) == -1)
		return 2;

	/*
	convert_coords_into_string_params;

	// get the dest coords

	uint8_t *sqlerr = NULL;
	uint8_t *room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET desc = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
			get_player_store(socket), get_player_name(socket), param_x, param_y, param_z);
			*/
	printf("%d %d %d\n", destroom.x, destroom.y, destroom.z);
	// need a mirrored version too

	/*
	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	*/
	return EXIT_SUCCESS;
}
