#include "do_cmd_action.h"

static void do_room_cmd(const int32_t socket, struct Command *info);
static void do_system_cmd(const int32_t socket, struct Command *info);
static void do_info_cmd(const int32_t socket, struct Command *info);
static void do_movement_cmd(const int32_t socket, struct Command *info);
static void do_travel_cmd(const int32_t socket, struct Command *info);

int32_t do_cmd_action(const int32_t socket, struct Command *info)
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

static void do_system_cmd(const int32_t socket, struct Command *info)
{
	if (info->subtype == SYS_SAY) {
		print_player_speech(socket);
	} else if (info->subtype == SYS_QUIT) {
		shutdown_socket(socket);
	}
}

static void do_info_cmd(const int32_t socket, struct Command *info)
{
	if (info->subtype == INFO_ROOM)
		print_to_player(socket, SHOWROOM);
	if (info->subtype == INFO_COMMANDS)
		print_to_player(socket, SHOWCMDS);
	if (info->subtype == INFO_PLAYERS)
		print_to_player(socket, LISTPLAYERS);
	if (info->subtype == INFO_MAP)
		printf("ADD ME\n");
}

static void do_room_cmd(const int32_t socket, struct Command *info)
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

static void do_movement_cmd(const int32_t socket, struct Command *info)
{
	int32_t rv = 0;
	struct Coordinates original_coords = get_player_coords(socket);

	struct Coordinates destination_coords;
	destination_coords.x = destination_coords.y = destination_coords.z = 0;

	if (info->subtype == MOVEMENT_NORTH) {
		destination_coords.y = original_coords.y + 1;
	} else if (info->subtype == MOVEMENT_EAST) {
		destination_coords.x = original_coords.x + 1;
	} else if (info->subtype == MOVEMENT_SOUTH) {
		destination_coords.y = original_coords.y - 1;
	} else if (info->subtype == MOVEMENT_WEST) {
		destination_coords.x = original_coords.x - 1;
	} else if (info->subtype == MOVEMENT_DOWN) {
		destination_coords.z = original_coords.z - 1;
	} else if (info->subtype == MOVEMENT_UP) {
		destination_coords.z = original_coords.z + 1;
	} else if (info->subtype == MOVEMENT_NORTHWEST) {
		destination_coords.x = original_coords.x - 1;
		destination_coords.y = original_coords.y + 1;
	} else if (info->subtype == MOVEMENT_NORTHEAST) {
		destination_coords.x = original_coords.x + 1;
		destination_coords.y = original_coords.y + 1;
	} else if (info->subtype == MOVEMENT_SOUTHWEST) {
		destination_coords.x = original_coords.x - 1;
		destination_coords.y = original_coords.y - 1;
	} else if (info->subtype == MOVEMENT_SOUTHEAST) {
		destination_coords.x = original_coords.x + 1;
		destination_coords.y = original_coords.y - 1;
	}

	rv = lookup_room_exits(socket, destination_coords);

	if (rv == -1) {
		rv = print_to_player(socket, INVALDIR);
		return;
	} else if (rv == -2) { // send them back to origin room, somewhere they shouldn't be
		rv = print_to_player(socket, INVALDIR);
		destination_coords.x = 0;
		destination_coords.y = 0;
		destination_coords.z = 0;
	}

	rv = print_to_player(socket, info->subtype);
	if (rv == 0) {
		print_to_player(socket, SHOWROOM);
		adjust_player_location(socket, destination_coords);
	}
}

static void do_travel_cmd(const int32_t socket, struct Command *info)
{
	if (info->subtype == TRAVEL_GOTO)
		printf("ADD ME\n");
		
	if (info->subtype == TRAVEL_SWAP)
		printf("ADD ME\n");
}
