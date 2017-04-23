#include "player_movement.h"

int32_t adjust_player_location(const int32_t socket, struct coordinates coords)
{
	if (coords.x == -1 && coords.y == -1 && coords.z == -1)
		coords.x = coords.y = coords.z = 0;

	convert_coords_into_string_params(coords.x, coords.y, coords.z);

	return run_sql(sqlite3_mprintf(
		"UPDATE PLAYERS SET x = %Q, y = %Q, z = %Q WHERE name = %Q;", 
		 param_x, param_y, param_z, (char *)get_player_name(socket)), 0, DB_PLAYER);
}

int32_t ensure_player_moving_valid_dir(const int32_t socket, const uint8_t *command)
{
	struct command *info = get_command_info(command);
	const int32_t type = info->type;

	free(info);

	if (type == MOVEMENT)
		 return EXIT_SUCCESS;

	print_to_player(socket, INVALDIR);
	print_to_player(socket, PRINT_EXITING_CMD_WAIT);

	reset_player_state(socket);

	return EXIT_FAILURE;
}
