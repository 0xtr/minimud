#include "remove_room.h"

static void check_exits_and_adjust(struct coordinates coords, struct room_atom *room);

#define player_is_not_owner memcmp(map->owner, get_player_name(socket), \
		strlen((char *)map->owner)) 
#define is_in_room(p,c) \
	p.x == c.x && p.y == c.y && p.z == c.z

int32_t remove_room(const int32_t socket)
{
	struct coordinates coords = get_player_coords(socket);
	struct room_atom *map = lookup_room(coords);

	if (map == NULL)
		return -1;

	if (player_is_not_owner) {
		free(map);
		return -2;
	}

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	int32_t rv = run_sql(sqlite3_mprintf(
			"DELETE FROM ROOMS WHERE x LIKE %Q AND y LIKE %Q AND z LIKE %Q;",
			param_x, param_y, param_z), 0, DB_ROOM);
	if (rv == EXIT_FAILURE) {
		free(map);
		return EXIT_FAILURE;
	}

	check_exits_and_adjust(coords, map);

	free(map);

	return EXIT_SUCCESS;
}

static void check_exits_and_adjust(struct coordinates coords, struct room_atom *room)
{
	int32_t evacuate_to = 0;

	for (size_t i = 0; i < 10; ++i) {
		if (room->exits[i] == -1)
			continue;

		int32_t val = 0;
		struct room_atom *target = lookup_room_by_id(room->exits[i]);

		if (target == NULL)
			continue;

		if (evacuate_to == 0)
			evacuate_to = target->id;

		printf("finding linked room: id %d\n", target->id);

		if (i > 0)
			val = i + 2;
		printf("removing link %d %s\n", val, get_movement_str(val));

		unlink_rooms(val, room, target);
	}

	assert(remove_players_from_room(coords, evacuate_to) == EXIT_SUCCESS);
}

int32_t remove_players_from_room(const struct coordinates coords, const int32_t r_id)
{
	struct query_matches *qmatches = players_in_room(coords);
	struct room_atom *room = lookup_room_by_id(r_id);

	for (size_t i = 0; i < qmatches->matches; ++i) {

		const int32_t this_socket = get_socket_by_id(qmatches->ids[i]);
		struct coordinates this_player = get_player_coords(this_socket);

		if (!(is_in_room(this_player, coords)))
			continue;

		print_to_player(this_socket, PRINT_REMOVED_FROM_ROOM);

		adjust_player_location(this_socket, room->coords);

		print_to_player(this_socket, SHOWROOM);
	}

	free(room);
	free(qmatches);

	return EXIT_SUCCESS;
}

