#include "room_adjustments.h"

#define player_is_not_owner strcmp((char *)map->owner, (char *)get_player_name(socket)) 

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

// TODO: split down
int32_t adjust_room_details(const int32_t adjusting, const int32_t socket, const struct Coordinates coords)
{
	uint8_t *sqlerr = NULL;
	uint8_t *room   = NULL;

	if (compare_room_owner(socket, coords) == -1)
		return -3;

	if (adjusting == ADJUSTING_ROOM_DESC) {
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rdesc = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_store(socket), get_player_name(socket), (char)coords.x, (char)coords.y, (char)coords.z);
	} else if (adjusting == ADJUSTING_ROOM_NAME) {
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rname = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_store(socket), get_player_name(socket), (char)coords.x, (char)coords.y, (char)coords.z);
	} else if (adjusting == ADJUSTING_ROOM_EXIT) {
		// room = adjusting_room_exit(socket);
	} else if (adjusting == ADJUSTING_ROOM_FLAG) {
		// got to get current, then add to rflags
		// room = adjusting_room_flag(socket);
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rflags = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_name(socket), (char)coords.x, (char)coords.y, (char)coords.z);
	}

	lookup_room_exits(-1, coords);

	if (sqlite3_exec(get_roomdb(), (char *)room, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	return EXIT_SUCCESS;
}

int32_t get_opposite_dir(const uint8_t *dir)
{
	const uint8_t VALID_DIRECTIONS[10][10] = {
		"north", "south", "west", "east", "northeast", "southwest", "northwest",
		"southeast", "up", "down"
	};

	size_t i;
	_Bool found = 0;
	for (i = 0; i < 10; ++i) {
		if (strcmp((char *)VALID_DIRECTIONS[i], (char *)dir) == 0) {
			found = true;
			break;
		}
	}
	if (found == false)
		return EXIT_FAILURE;

	if (strcmp((char *)dir, "south") == 0	 ||
		strcmp((char *)dir, "east") == 0	  ||
		strcmp((char *)dir, "southwest") == 0 ||
		strcmp((char *)dir, "southeast") == 0 ||
		strcmp((char *)dir, "down") == 0) {
		return i - 1;
	} else {
		return i + 1;
	}
	return EXIT_FAILURE;
}

int32_t adjust_room_exit(const int32_t socket, const struct Coordinates player_coords, const struct Coordinates new_coords)
{
	printf("%d %d %d\n", socket, player_coords.x, new_coords.x);
	// TODO: use newroom 
	/*
	if (strlen((char *)get_player_store(socket)) == 0 || get_player_store(socket) == NULL)
		return -1;

	const int32_t direction = get_direction_as_number(get_player_store(socket));

	struct RoomRecord *map = lookup_room(x, y, z, socket);

	uint8_t *room = sqlite3_mprintf("UPDATE ROOMS SET %Q = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_dir_string(direction), reverse_of_current(direction, reverse), get_player_name(socket), (char)x, (char)y, (char)z);
			*/

	//free_room(map);
	// stuff
	return EXIT_SUCCESS;
}
