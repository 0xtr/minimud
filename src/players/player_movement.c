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

int32_t ensure_player_moving_valid_dir(const int32_t socket, const uint8_t *command)
{
	struct Command *info = (struct Command *)malloc(sizeof(struct Command));
	info = is_movement_cmd(command, info);
	enum COMMAND_TYPES type = info->type;
	free(info);
	if (type == MOVEMENT)
		 return EXIT_SUCCESS;

	print_to_player(socket, INVALDIR);\
	print_to_player(socket, PRINT_EXITING_CMD_WAIT);\
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	return EXIT_FAILURE;
}
