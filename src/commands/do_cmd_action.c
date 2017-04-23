#include "do_cmd_action.h"

static void do_room_cmd(const int32_t socket, struct command *info);
static void do_system_cmd(const int32_t socket, struct command *info);
static void do_info_cmd(const int32_t socket, struct command *info);
static void do_movement_cmd(const int32_t socket, struct command *info);
static void do_travel_cmd(const int32_t socket, struct command *info);

int32_t do_cmd_action(const int32_t socket, struct command *info)
{
	if (info->type == MOVEMENT)
		do_movement_cmd(socket, info);

	if (info->type == SYSTEM_ACTION)
		do_system_cmd(socket, info);

	if (info->type == ROOM_CHANGE)
		do_room_cmd(socket, info);

	if (info->type == TRAVEL_ACTION)
		do_travel_cmd(socket, info);

	if (info->type == INFO_REQUEST)
		do_info_cmd(socket, info);

	free(info);

	return EXIT_SUCCESS; 
}

static void do_system_cmd(const int32_t socket, struct command *info)
{
	if (info->subtype == SYS_SAY) {
		print_player_speech(socket);
	} else if (info->subtype == SYS_QUIT) {
		shutdown_socket(socket);
	}
}

static void do_info_cmd(const int32_t socket, struct command *info)
{
	if (info->subtype == INFO_ROOM || info->subtype == INFO_ROOM2)
		print_to_player(socket, SHOWROOM);
	if (info->subtype == INFO_COMMANDS)
		print_to_player(socket, SHOWCMDS);
	if (info->subtype == INFO_PLAYERS)
		print_to_player(socket, LISTPLAYERS);
	if (info->subtype == INFO_MAP)
		printf("ADD ME\n");
}

static void do_room_cmd(const int32_t socket, struct command *info)
{
	if (info->subtype == ROOM_SET_NAME) {
		print_to_player(socket, PRINT_PROVIDE_NEW_ROOM_NAME);
		set_player_wait_state(socket, WAIT_ENTER_NEW_ROOM_NAME);
		set_player_holding_for_input(socket, 1);
	} else if (info->subtype == ROOM_SET_DESC) {
		print_to_player(socket, PRINT_PROVIDE_NEW_ROOM_DESC);
		set_player_wait_state(socket, WAIT_ENTER_NEW_ROOM_DESC);
		set_player_holding_for_input(socket, 1);
	} else if (info->subtype == ROOM_SET_EXIT) {
		print_to_player(socket, PRINT_PROVIDE_ROOM_EXIT_NAME);
		set_player_wait_state(socket, WAIT_ENTER_EXIT_NAME);
		set_player_holding_for_input(socket, 1);
	} else if (info->subtype == ROOM_SET_FLAG) {
		print_to_player(socket, PRINT_PROVIDE_ROOM_FLAG_NAME);
		set_player_wait_state(socket, WAIT_ENTER_FLAG_NAME);
		set_player_holding_for_input(socket, 1);
	} else if (info->subtype == ROOM_MK) {
		print_to_player(socket, PRINT_ROOM_CREATION_GIVE_DIR);
		set_player_wait_state(socket, WAIT_ROOM_CREATION_DIR);
		set_player_holding_for_input(socket, 1);
	} else if (info->subtype == ROOM_RM) {
		print_to_player(socket, PRINT_ROOM_REMOVAL_CHECK);
		set_player_wait_state(socket, WAIT_ROOM_REMOVAL_CHECK);
		set_player_holding_for_input(socket, 1);
	}
}

static void do_movement_cmd(const int32_t socket, struct command *info)
{
	int32_t dir = 0;
	int32_t rv = 0;
	struct coordinates origin = get_player_coords(socket);
	struct coordinates destination;

	destination.x = destination.y = destination.z = 0;

	if (info->subtype == DIR_NORTH) {
		dir = DIR_NORTH;
		destination.y = origin.y + 1;
	} else if (info->subtype == DIR_EAST) {
		dir = DIR_EAST;
		destination.x = origin.x + 1;
	} else if (info->subtype == DIR_SOUTH) {
		dir = DIR_SOUTH;
		destination.y = origin.y - 1;
	} else if (info->subtype == DIR_WEST) {
		dir = DIR_WEST;
		destination.x = origin.x - 1;
	} else if (info->subtype == DIR_DOWN) {
		dir = DIR_DOWN;
		destination.z = origin.z - 1;
	} else if (info->subtype == DIR_UP) {
		dir = DIR_UP;
		destination.z = origin.z + 1;
	} else if (info->subtype == DIR_NORTHWEST) {
		dir = DIR_NORTHWEST;
		destination.x = origin.x - 1;
		destination.y = origin.y + 1;
	} else if (info->subtype == DIR_NORTHEAST) {
		dir = DIR_NORTHEAST;
		destination.x = origin.x + 1;
		destination.y = origin.y + 1;
	} else if (info->subtype == DIR_SOUTHWEST) {
		dir = DIR_SOUTHWEST;
		destination.x = origin.x - 1;
		destination.y = origin.y - 1;
	} else if (info->subtype == DIR_SOUTHEAST) {
		dir = DIR_SOUTHEAST;
		destination.x = origin.x + 1;
		destination.y = origin.y - 1;
	}

	rv = lookup_room_exits(origin, destination);

	if (rv == -1) {
		rv = print_to_player(socket, INVALDIR);
		return;
	} else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
		rv = print_to_player(socket, INVALDIR);
		destination.x = 0;
		destination.y = 0;
		destination.z = 0;
	}

	rv = print_to_player(socket, dir);
	if (rv == 0) {
		adjust_player_location(socket, destination);
		print_to_player(socket, SHOWROOM);
	}
}

static void do_travel_cmd(const int32_t socket, struct command *info)
{
	if (info->subtype == TRAVEL_GOTO)
		printf("ADD ME %d\n", socket);
		
	if (info->subtype == TRAVEL_SWAP)
		printf("ADD ME %d\n", socket);
}
