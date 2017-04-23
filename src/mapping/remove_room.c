#include "remove_room.h"

static void check_exits_and_adjust(struct room_atom *room);

#define player_is_not_owner strcmp((char *)map->owner, (char *)get_player_name(socket)) 

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

	check_exits_and_adjust(map);
	assert(remove_players_from_room(coords) == EXIT_SUCCESS);

	free(map);
	return EXIT_SUCCESS;
}

static void check_exits_and_adjust(struct room_atom *room)
{
	for (size_t i = 0; i < 10; ++i) {
		if (room->exits[i] != -1) {
			struct room_atom *target = lookup_room_by_id(room->exits[i]);
			printf("finding linked room: id %d\n", target->id);

			if (target == NULL)
				continue;

			adjust_room_exit(i, room, target);
		}
	}
}

