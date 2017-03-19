#include "player_movement.h"

int32_t adjust_player_location(const int32_t socket, int32_t x, int32_t y, int32_t z)
{
	if (x == -1 && y == -1 && z == -1) {
		x = y = z = 0;
	}
	uint8_t *sqlerr = NULL;
	uint8_t *querystr = (uint8_t*)sqlite3_mprintf("UPDATE PLAYERS SET x = %Q, y = %Q, z = %Q WHERE name = %Q;", 
		            (char)x, (char)y, (char)z, (char*)get_player_name(socket));

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
	int32_t x = get_player_coord(X_COORD_REQUEST, socket);
	int32_t y = get_player_coord(Y_COORD_REQUEST, socket);
	int32_t z = get_player_coord(Z_COORD_REQUEST, socket);
	int32_t xadj, yadj, zadj;

	xadj = yadj = zadj = 0;

	// new funcs here
	// get a struct with the fields?
	if (DIRECTION == NORTH_DIR) {
		y = y + 1;
		yadj = 1;
	} else if (DIRECTION == EAST_DIR) {
		x = x + 1;
		xadj = 1;
	} else if (DIRECTION == SOUTH_DIR) {
		y = y - 1;
		yadj = -1;
	} else if (DIRECTION == WEST_DIR) {
		x = x - 1;
		xadj = -1;
	} else if (DIRECTION == DOWN_DIR) {
		z = z - 1;
		zadj = -1;
	} else if (DIRECTION == UP_DIR) {
		z = z + 1;
		zadj = 1;
	} else if (DIRECTION == NORTHWEST_DIR) {
		x = x - 1;
		y = y + 1;
		xadj = -1;
		yadj = 1;
	} else if (DIRECTION == NORTHEAST_DIR) {
		x = x + 1;
		y = y + 1;
		xadj = 1;
		yadj = 1;
	} else if (DIRECTION == SOUTHWEST_DIR) {
		y = y - 1;
		x = x - 1;
		xadj = -1;
		yadj = -1;
	} else if (DIRECTION == SOUTHEAST_DIR) {
		x = x + 1;
		y = y - 1;
		xadj = 1;
		yadj = -1;
	} else if (DIRECTION == RETURN_ORIGIN_DIR) {
		x = -1;
		y = -1;
		z = -1;
	}


	rv = lookup_room_exits(socket, xadj, yadj, zadj);

	if (rv == -1) {
		rv = print_to_player(socket, INVALDIR);
		return -1;
	} else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
		rv = print_to_player(socket, INVALDIR);
		x = -1;
		y = -1;
		z = -1;
	}

	rv = print_to_player(socket, DIRECTION);
	if (rv == 2) {
		print_to_player(socket, SHOWROOM);
		return adjust_player_location(socket, x, y, z);
	} else if (rv == 0) {
		print_to_player(socket, SHOWROOM);
		return adjust_player_location(socket, -1, -1, -1);
	} else {
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
