#include "do_cmd_action.h"

static void do_room_cmd(struct player_live_record *player, struct command *info);
static void do_system_cmd(struct player_live_record *player, struct command *info);
static void do_info_cmd(struct player_live_record *player, struct command *info);
static void do_movement_cmd(struct player_live_record *player, struct command *info);
static void do_travel_cmd(struct player_live_record *player, struct command *info);

int32_t do_cmd_action(struct player_live_record *player, struct command *info)
{
	if (info->type == MOVEMENT)
		do_movement_cmd(player, info);

	if (info->type == SYSTEM_ACTION)
		do_system_cmd(player, info);

	if (info->type == ROOM_CHANGE)
		do_room_cmd(player, info);

	if (info->type == TRAVEL_ACTION)
		do_travel_cmd(player, info);

	if (info->type == INFO_REQUEST)
		do_info_cmd(player, info);

	free(info);

	return EXIT_SUCCESS; 
}

static void do_system_cmd(struct player_live_record *player, struct command *info)
{
	if (info->subtype == SYS_SAY) {
		print_player_speech(player);
	} else if (info->subtype == SYS_QUIT) {
		shutdown_socket(player);
	}
}

static void do_info_cmd(struct player_live_record *player, struct command *info)
{
	if (info->subtype == INFO_ROOM || info->subtype == INFO_ROOM2) {
		struct room_db_record *room = lookup_room(get_player_coords(player));

		print_room_to_player(player, room);

		free(room);
	}

	if (info->subtype >= INFO_COMMANDS && info->subtype <= INFO_COMMANDS3)
		print_to_player(player, SHOWCMDS);
	if (info->subtype == INFO_PLAYERS)
		print_to_player(player, LISTPLAYERS);
	if (info->subtype == INFO_MAP)
		printf("ADD ME\n");
}

static void do_room_cmd(struct player_live_record *player, struct command *info)
{
	if (info->subtype == ROOM_SET_NAME) {
		print_to_player(player, PRINT_PROVIDE_NEW_ROOM_NAME);
		player->holding_for_input = 1;
		player->wait_state = WAIT_ENTER_NEW_ROOM_NAME;
	} else if (info->subtype == ROOM_SET_DESC) {
		print_to_player(player, PRINT_PROVIDE_NEW_ROOM_DESC);
		player->wait_state = WAIT_ENTER_NEW_ROOM_DESC;
		player->holding_for_input = 1;
	} else if (info->subtype == ROOM_SET_EXIT) {
		print_to_player(player, PRINT_PROVIDE_ROOM_EXIT_NAME);
		player->wait_state = WAIT_ENTER_EXIT_NAME;
		player->holding_for_input = 1;
	} else if (info->subtype == ROOM_SET_FLAG) {
		print_to_player(player, PRINT_PROVIDE_ROOM_FLAG_NAME);
		player->wait_state = WAIT_ENTER_FLAG_NAME;
		player->holding_for_input = 1;
	} else if (info->subtype == ROOM_MK) {
		print_to_player(player, PRINT_ROOM_CREATION_GIVE_DIR);
		player->wait_state = WAIT_ROOM_CREATION_DIR;
		player->holding_for_input = 1;
	} else if (info->subtype == ROOM_RM) {
		print_to_player(player, PRINT_ROOM_REMOVAL_CHECK);
		player->wait_state = WAIT_ROOM_REMOVAL_CHECK;
		player->holding_for_input = 1;
	}
}

static void do_movement_cmd(struct player_live_record *player, struct command *info)
{
	int32_t dir = 0;
	int32_t rv = 0;
	struct coordinates origin = get_player_coords(player);
	struct coordinates destination = {0};

	//destination.x = destination.y = destination.z = 0;

	if (is_dir(info->subtype, DIR_NORTH)) {
		dir = DIR_NORTH;
		destination.y = origin.y + 1;
	} else if (is_dir(info->subtype, DIR_EAST)) {
		dir = DIR_EAST;
		destination.x = origin.x + 1;
	} else if (is_dir(info->subtype, DIR_SOUTH)) {
		dir = DIR_SOUTH;
		destination.y = origin.y - 1;
	} else if (is_dir(info->subtype, DIR_WEST)) {
		dir = DIR_WEST;
		destination.x = origin.x - 1;
	} else if (is_dir(info->subtype, DIR_DOWN)) {
		dir = DIR_DOWN;
		destination.z = origin.z - 1;
	} else if (is_dir(info->subtype, DIR_UP)) {
		dir = DIR_UP;
		destination.z = origin.z + 1;
	} else if (is_dir(info->subtype, DIR_NORTHWEST)) {
		dir = DIR_NORTHWEST;
		destination.x = origin.x - 1;
		destination.y = origin.y + 1;
	} else if (is_dir(info->subtype, DIR_NORTHEAST)) {
		dir = DIR_NORTHEAST;
		destination.x = origin.x + 1;
		destination.y = origin.y + 1;
	} else if (is_dir(info->subtype, DIR_SOUTHWEST)) {
		dir = DIR_SOUTHWEST;
		destination.x = origin.x - 1;
		destination.y = origin.y - 1;
	} else if (is_dir(info->subtype, DIR_SOUTHEAST)) {
		dir = DIR_SOUTHEAST;
		destination.x = origin.x + 1;
		destination.y = origin.y - 1;
	}

	struct room_db_record *dest_room = lookup_room(destination);
	if (dest_room == NULL) {
		// do something
	}
	rv = lookup_room_exits(origin, dest_room);

	if (rv == -1) {
		rv = print_to_player(player, INVALDIR);
		return;
	} else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
		rv = print_to_player(player, INVALDIR);
		destination.x = 0;
		destination.y = 0;
		destination.z = 0;
	}

	rv = print_to_player(player, dir);
	if (rv == 0) {
		adjust_player_location(player, dest_room->id);

		print_room_to_player(player, dest_room);
	}

	if (dest_room != NULL)
		free(dest_room);
}

static void do_travel_cmd(struct player_live_record *player, struct command *info)
{
	if (info->subtype == TRAVEL_GOTO)
		printf("ADD ME %d\n", player->socket_num);
		
	if (info->subtype == TRAVEL_SWAP)
		printf("ADD ME %d\n", player->socket_num);
}
