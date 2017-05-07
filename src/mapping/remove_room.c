#include "remove_room.h"

static int32_t exit_to_dir(const int32_t exit);
static void check_exits_and_adjust(struct coordinates coords, struct room_atom *room);

#define player_is_not_owner memcmp(map->owner, player->name, \
		strlen((char *)map->owner)) 

int32_t remove_room(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);
	struct room_atom *map = lookup_room(coords);

	if (map == NULL)
		return -1;

	if (player_is_not_owner) {
		free(map);
		return -2;
	}

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	check_exits_and_adjust(coords, map);

	int32_t rv = run_sql(sqlite3_mprintf(
			"DELETE FROM ROOMS WHERE x LIKE %Q AND y LIKE %Q AND z LIKE %Q;",
			param_x, param_y, param_z), 0, DB_ROOM);
	if (rv == EXIT_FAILURE) {
		free(map);
		return EXIT_FAILURE;
	}

	free(map);

	return EXIT_SUCCESS;
}

static void check_exits_and_adjust(struct coordinates coords, struct room_atom *room)
{
	int32_t evacuate_to = 0;

	for (size_t i = 0; i < 10; ++i) {
		if (room->exits[i] == -1)
			continue;

		struct room_atom *target = lookup_room_by_id(room->exits[i]);

		if (target == NULL)
			continue;

		if (evacuate_to == 0)
			evacuate_to = target->id;

		printf("finding linked room: id %d\n", target->id);

		int32_t actual = exit_to_dir(i);
		printf("removing link %d %s\n", actual, get_movement_str(actual));

		unlink_rooms(actual, room, target);

		free(target);
	}

	assert(remove_players_from_room(coords, room) == EXIT_SUCCESS);
}

static int32_t exit_to_dir(const int32_t exit)
{
	if (exit == NORTH_EXIT)
		return DIR_NORTH;
	if (exit == EAST_EXIT)
		return DIR_EAST;
	if (exit == SOUTH_EXIT)
		return DIR_SOUTH;
	if (exit == WEST_EXIT)
		return DIR_WEST;
	if (exit == UP_EXIT)
		return DIR_UP;
	if (exit == DOWN_EXIT)
		return DIR_DOWN;
	if (exit == NORTHEAST_EXIT)
		return DIR_NORTHEAST;
	if (exit == SOUTHEAST_EXIT)
		return DIR_SOUTHEAST;
	if (exit == SOUTHWEST_EXIT)
		return DIR_SOUTHWEST; 
	if (exit == NORTHWEST_EXIT)
		return DIR_NORTHWEST;
	return -1;
}

int32_t remove_players_from_room(const struct coordinates coords, struct room_atom *room)
{
	struct query_matches *qmatches = players_in_room(room->id);

	printf("matches to remove %lu\n", qmatches->matches);

	for (size_t i = 0; i < qmatches->matches; ++i) {
		if (qmatches->ids[i] == 0)
			break;

		struct player_live_record *player = get_player_by_id(qmatches->ids[i]);
		printf("player %s %d\n", player->name, player->socket_num);
		struct coordinates this = get_player_coords(player);

		if (!(this.x == coords.x && this.y == coords.y 
					&& this.z == coords.z))
			continue;

		print_to_player(player, PRINT_REMOVED_FROM_ROOM);

		struct room_atom *room = lookup_room(coords);

		adjust_player_location(player, room->id);
		// check results

		free(room); // get the updated room image
		room = lookup_room(get_player_coords(player));

		print_room_to_player(player, room);

		free(room);
	}

	free(qmatches);

	return EXIT_SUCCESS;
}

