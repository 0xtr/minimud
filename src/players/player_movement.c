#include "player_movement.h"

int32_t adjust_player_location(const int32_t socket, struct Coordinates coords)
{
	if (coords.x == -1 && coords.y == -1 && coords.z == -1) {
		coords.x = coords.y = coords.z = 0;
	}
	uint8_t *sqlerr = NULL;
	uint8_t *querystr = (uint8_t*)sqlite3_mprintf("UPDATE PLAYERS SET x = %Q, y = %Q, z = %Q WHERE name = %Q;", 
		            (char)coords.x, (char)coords.y, (char)coords.z, (char*)get_player_name(socket));

	if (sqlite3_exec(get_playerdb(), (char*)querystr, player_callback, 0, (char**)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE player location adjustment error:\n%s\n", sqlite3_errmsg(get_playerdb()));
		print_to_player(socket, INVALDIR);
		sqlite3_free(querystr);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(querystr);

	return EXIT_SUCCESS;
}

int32_t move_player(const int32_t socket, const int32_t DIRECTION)
{
	int32_t rv = 0;
	struct Coordinates original_coords = get_player_coords(socket);

	struct Coordinates destination_coords;
	destination_coords.x = destination_coords.y = destination_coords.z = 0;

	if (DIRECTION == NORTH_DIR) {
		destination_coords.y = original_coords.y + 1;
	} else if (DIRECTION == EAST_DIR) {
		destination_coords.x = original_coords.x + 1;
	} else if (DIRECTION == SOUTH_DIR) {
		destination_coords.y = original_coords.y - 1;
	} else if (DIRECTION == WEST_DIR) {
		destination_coords.x = original_coords.x - 1;
	} else if (DIRECTION == DOWN_DIR) {
		destination_coords.z = original_coords.z - 1;
	} else if (DIRECTION == UP_DIR) {
		destination_coords.z = original_coords.z + 1;
	} else if (DIRECTION == NORTHWEST_DIR) {
		destination_coords.x = original_coords.x - 1;
		destination_coords.y = original_coords.y + 1;
	} else if (DIRECTION == NORTHEAST_DIR) {
		destination_coords.x = original_coords.x + 1;
		destination_coords.y = original_coords.y + 1;
	} else if (DIRECTION == SOUTHWEST_DIR) {
		destination_coords.x = original_coords.x - 1;
		destination_coords.y = original_coords.y - 1;
	} else if (DIRECTION == SOUTHEAST_DIR) {
		destination_coords.x = original_coords.x + 1;
		destination_coords.y = original_coords.y - 1;
	} else if (DIRECTION == RETURN_ORIGIN_DIR) {
		destination_coords.x = -1;
		destination_coords.y = -1;
		destination_coords.z = -1;
	}

	rv = lookup_room_exits(socket, destination_coords);

	if (rv == -1) {
		rv = print_to_player(socket, INVALDIR);
		return -1;
	} else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
		rv = print_to_player(socket, INVALDIR);
		destination_coords.x = -1;
		destination_coords.y = -1;
		destination_coords.z = -1;
	}

	rv = print_to_player(socket, DIRECTION);
	if (rv == 0) {
		print_to_player(socket, SHOWROOM);
		return adjust_player_location(socket, destination_coords);
	} else {
		// TODO; clarify 
		return EXIT_FAILURE;
	}
}

int32_t ensure_player_moving_valid_dir(const int32_t socket, const uint8_t *command)
{
	if (is_direction(command))
		 return EXIT_SUCCESS;

	print_to_player(socket, INVALDIR);\
	print_to_player(socket, PRINT_EXITING_CMD_WAIT);\
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	return EXIT_FAILURE;
}
