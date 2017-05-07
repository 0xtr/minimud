#include "player_movement.h"

int32_t adjust_player_location(struct player_live_record *player, const int32_t rid)
{
	uint8_t rid_str[2 * sizeof(rid)] = {0};\
	snprintf((char *)rid_str, sizeof(rid), "%d", rid);\

	return run_sql(sqlite3_mprintf(
		"UPDATE PLAYERS SET loc_id = %Q WHERE name = %Q;", 
		 rid_str, (char *)player->name), 0, DB_PLAYER);
}

int32_t ensure_player_moving_valid_dir(struct player_live_record *player, const uint8_t *command)
{
	struct command *info = get_command_info(command);
	const int32_t type = info->type;

	free(info);

	if (type == MOVEMENT)
		 return EXIT_SUCCESS;

	print_to_player(player, INVALDIR);
	print_to_player(player, PRINT_EXITING_CMD_WAIT);

	reset_player_state(player);

	return EXIT_FAILURE;
}
