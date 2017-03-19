#include "room_adjustments.h"

static int32_t adjusting_room_exit(const int32_t socket, const struct Coordinates player_coords, const struct Coordinates new_coords);
static void check_exits_and_adjust(const int32_t socket, const struct RoomRecord *map, struct Coordinates new_coords);

#define player_is_not_owner strcmp((char *)map->owner, (char *)get_player_name(socket)) 

int32_t remove_room(const int32_t socket)
{
	uint8_t *sqlerr = NULL;
	struct Coordinates coords = get_player_coords(socket);
	struct RoomRecord *map = lookup_room(coords);

	if (map == NULL)
		return -1;

	if (player_is_not_owner) {
		free_room(map);
		return -2;
	}

	uint8_t *querystr = (uint8_t *)sqlite3_mprintf(
			"DELETE FROM ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;",
			(char)coords.x, (char)coords.y, (char)coords.z);

	if (sqlite3_exec(get_roomdb(), (char *)querystr, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 failure in remove_room; could not delete the room:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(querystr);
		sqlite3_free(sqlerr);
		free_room(map);
		return -3;
	}

	sqlite3_free(querystr);

	check_exits_and_adjust(socket, map, coords);
	assert(remove_players_from_room(coords) == EXIT_SUCCESS);

	free_room(map);
	return EXIT_SUCCESS;
}

static void check_exits_and_adjust(const int32_t socket, const struct RoomRecord *map, struct Coordinates new_coords)
{
	struct Coordinates player_coords = new_coords;
	if (map->north == true)
		new_coords.y += 1;
	if (map->south == true)
		new_coords.y -= 1;
	if (map->east == true)
		new_coords.x += 1;
	if (map->west == true)
		new_coords.x -= 1;

	if (map->up == true)
		new_coords.z += 1;
	if (map->down == true)
		new_coords.z -= 1;

	if (map->northeast == true)
		new_coords.x += 1, new_coords.y += 1;
	if (map->southeast == true)
		new_coords.x += 1, new_coords.y -= 1;
	if (map->southwest == true)
		new_coords.x -= 1, new_coords.y -= 1;
	if (map->northwest == true)
		new_coords.x -= 1, new_coords.y += 1;

	adjusting_room_exit(socket, player_coords, new_coords);
}

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

int32_t get_direction_as_number(const uint8_t *dir)
{
	if (strcmp((char *)dir, "north") || strcmp((char *)dir,"North"))
		return NORTH_DIR;
	if (strcmp((char *)dir, "south") || strcmp((char *)dir,"South"))
		return SOUTH_DIR;
	if (strcmp((char *)dir, "east") || strcmp((char *)dir,"East"))
		return EAST_DIR;
	if (strcmp((char *)dir, "west") || strcmp((char *)dir,"West"))
		return WEST_DIR;
	if (strcmp((char *)dir, "up") || strcmp((char *)dir,"Up"))
		return UP_DIR;
	if (strcmp((char *)dir, "down") || strcmp((char *)dir,"Down"))
		return DOWN_DIR;
	if (strcmp((char *)dir, "northeast") || strcmp((char *)dir,"Northeast"))
		return NORTHEAST_DIR;
	if (strcmp((char *)dir, "southeast") || strcmp((char *)dir,"Southeast"))
		return SOUTHEAST_DIR;
	if (strcmp((char *)dir, "southwest") || strcmp((char *)dir,"Southwest"))
		return SOUTHWEST_DIR;
	if (strcmp((char *)dir, "northwest") || strcmp((char *)dir,"Northwest"))
		return NORTHWEST_DIR;
	if (strcmp((char *)dir, "return") || strcmp((char *)dir, "Return"))
		return RETURN_ORIGIN_DIR;
	return EXIT_FAILURE;
}

uint8_t *get_dir_string(const int32_t dir)
{
	if (dir == RETURN_ORIGIN_DIR)
		return (uint8_t *)"back to the core.";
	if (dir == NORTH_DIR)
		return (uint8_t *)"north";
	if (dir == SOUTH_DIR)
		return (uint8_t *)"south";
	if (dir == EAST_DIR)
		return (uint8_t *)"east";
	if (dir == WEST_DIR)
		return (uint8_t *)"west";
	if (dir == UP_DIR)
		return (uint8_t *)"up";
	if (dir == DOWN_DIR)
		return (uint8_t *)"down";
	if (dir == NORTHEAST_DIR)
		return (uint8_t *)"northeast";
	if (dir == SOUTHEAST_DIR)
		return (uint8_t *)"southeast";
	if (dir == SOUTHWEST_DIR)
		return (uint8_t *)"southwest";
	if (dir == NORTHWEST_DIR)
		return (uint8_t *)"northwest";
	return (uint8_t *)"EMPTY";
}

static int32_t adjusting_room_exit(const int32_t socket, const struct Coordinates player_coords, const struct Coordinates new_coords)
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
