#include "remove_room.h"

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

	adjust_room_exit(socket, player_coords, new_coords);
}

