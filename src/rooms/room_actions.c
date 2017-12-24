#include "room_actions.h"

#define player_is_not_owner memcmp(map->owner, player->name, strlen((char *)map->owner)) 

static int32_t exit_to_dir(const int32_t exit);
static void check_exits_and_adjust(struct coordinates coords, struct room_db_record *room);

int32_t adjust_room_name(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);
	if (compare_room_owner(player, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords);

	return run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET name = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		player->store, player->name, param_x, param_y, param_z), 0, DB_ROOM);
}

int32_t adjust_room_desc(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);
	if (compare_room_owner(player, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords);

	return run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET desc = %Q, last_modified_by = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		player->store, player->name, param_x, param_y, param_z), 0, DB_ROOM);
}

int32_t adjust_room_flag(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);
	if (compare_room_owner(player, coords) == -1)
		return 2;

	convert_coords_into_string_params(coords);

	return run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET flags = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		player->name, param_x, param_y, param_z), 0, DB_ROOM);
}

int32_t link_rooms(const int32_t dir, struct room_db_record *existing, struct room_db_record *newroom)
{
	convert_coords_into_string_params(existing->coords);

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

	return run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET %Q = %Q WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_opposite_str(dir), id, param_x, param_y, param_z), 0, DB_ROOM);
}

int32_t unlink_rooms(const int32_t dir, struct room_db_record *existing, struct room_db_record *newroom)
{
	convert_coords_into_string_params(existing->coords);

	int32_t rv = run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET %Q = '-1' WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_movement_str(dir), param_x, param_y, param_z), 0, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return rv;

	snprintf((char *)param_x, sizeof(newroom->coords.x), "%d", newroom->coords.x);
	snprintf((char *)param_y, sizeof(newroom->coords.y), "%d", newroom->coords.y);
	snprintf((char *)param_z, sizeof(newroom->coords.z), "%d", newroom->coords.z);

	return run_sql(sqlite3_mprintf(
		"UPDATE ROOMS SET %Q = '-1' WHERE x = %Q AND y = %Q AND z = %Q;", 
		get_opposite_str(dir), param_x, param_y, param_z), 0, DB_ROOM);
}

struct room_db_record *insert_room(struct room_blueprint rconfig)
{
	uint8_t param_x[2 * sizeof(rconfig.coords.x)] = {0};
	uint8_t param_y[2 * sizeof(rconfig.coords.y)] = {0};
	uint8_t param_z[2 * sizeof(rconfig.coords.z)] = {0};
	snprintf((char *)param_x, sizeof(rconfig.coords.x), "%d", rconfig.coords.x);
	snprintf((char *)param_y, sizeof(rconfig.coords.y), "%d", rconfig.coords.y);
	snprintf((char *)param_z, sizeof(rconfig.coords.z), "%d", rconfig.coords.z);

	run_sql(sqlite3_mprintf(
		"INSERT INTO ROOMS (name, desc, x, y, z, north, east, south, west, up, down, "
		"northeast, southeast, southwest, northwest, owner, last_modified_by, flags) "
		"VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char *)rconfig.name, (char *)rconfig.desc, 
		param_x, param_y, param_z, 
		"-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", 
		(char *)rconfig.owner, (char *)rconfig.owner, (char *)rconfig.flags), 0, DB_ROOM);

	struct room_db_record *room = lookup_room(rconfig.coords);

	return room;
}

int32_t remove_room(struct player_live_record *player)
{
	struct coordinates coords = get_player_coords(player);
	struct room_db_record *map = lookup_room(coords);

	if (map == NULL)
		return -1;

	if (player_is_not_owner) {
		free(map);
		return -2;
	}

	//convert_coords_into_string_params(coords.x, coords.y, coords.z);
	convert_coords_into_string_params(coords);

	check_exits_and_adjust(coords, map);

	int32_t rv = run_sql(sqlite3_mprintf(
			"DELETE FROM ROOMS WHERE x = %Q AND y = %Q AND z = %Q;",
			param_x, param_y, param_z), 0, DB_ROOM);
	if (rv == EXIT_FAILURE) {
		free(map);
		return EXIT_FAILURE;
	}

	free(map);

	return EXIT_SUCCESS;
}

static void check_exits_and_adjust(struct coordinates coords, struct room_db_record *room)
{
	int32_t evacuate_to = 0;

	for (size_t i = 0; i < 10; ++i) {
		if (room->exits[i] == -1)
			continue;

		struct room_db_record *target = lookup_room_by_id(room->exits[i]);

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

int32_t remove_players_from_room(const struct coordinates coords, struct room_db_record *room)
{
	struct query_matches *qmatches = players_in_room(room->id);

	for (size_t i = 0; i < qmatches->matches; ++i) {
		if (qmatches->ids[i] == 0)
			break;

		struct player_live_record *player = get_player_by_id(qmatches->ids[i]);
		struct coordinates this = get_player_coords(player);

		if (!(this.x == coords.x && this.y == coords.y 
					&& this.z == coords.z))
			continue;

		print_to_player(player, PRINT_REMOVED_FROM_ROOM);

		struct room_db_record *room = lookup_room(coords);

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

struct room_db_record *lookup_room(const struct coordinates coords)
{
	convert_coords_into_string_params(coords);

	struct room_db_record *map = get_room();

	int32_t rv = run_sql(sqlite3_mprintf(
			"SELECT * FROM ROOMS WHERE x = %Q AND y = %Q AND z = %Q;", 
			param_x, param_y, param_z), map, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return NULL;

	return map;
}

struct room_db_record *lookup_room_by_id(const int32_t id)
{
	uint8_t idstr[2 * sizeof(id)] = {0};
	snprintf((char *)idstr, sizeof(id), "%d", id);

	struct room_db_record *map = get_room();

	int32_t rv = run_sql(sqlite3_mprintf(
			"SELECT * FROM ROOMS WHERE id = %Q;", idstr), map, DB_ROOM);

	if (rv == EXIT_FAILURE)
		return NULL;

	return map;
}

int32_t lookup_room_exits(const struct coordinates origin, struct room_db_record *dest_room)
{
	int32_t rv = 0;
	struct room_db_record *origin_room = lookup_room(origin);

	if (dest_room == NULL) {
		rv = -2;
		goto end;
	}

	if (has_exit_for_dir(origin_room, dest_room) == EXIT_FAILURE)
		rv = -1;

	end: 

	free(origin_room);

	return rv;
}

int32_t has_exit_for_dir(struct room_db_record *origin, struct room_db_record *dest)
{
	for (size_t i = 0; i < 10; ++i) {
		if (dest->exits[i] == -1)
			continue;

		if (memcmp(&origin->id, &dest->exits[i], sizeof(int32_t)) == 0)
			return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

int32_t lookup_room_name_from_coords(struct player_live_record *player, const struct coordinates coords)
{
	struct room_db_record *map = lookup_room(coords);

	if (map != NULL) {
		// get_room_details(map);
		set_player_buffer_replace(player, map->rname);
		free(map);
	} else {
		set_player_buffer_replace(player, "NULL SPACE");
	}

	return EXIT_SUCCESS;
}

int32_t compare_room_owner(struct player_live_record *player, const struct coordinates coords)
{
	int32_t rv = EXIT_SUCCESS;
	struct room_db_record *map = lookup_room(coords);

	if (strcmp((char*)player->name, (char*)map->owner) != 0)
		rv = EXIT_FAILURE;

	free(map);

	return rv;
}
