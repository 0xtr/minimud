#include "interpret_command.h"

static uint8_t *process_buffer(const size_t socket);
static _Bool handle_incoming_name(const int32_t socket, const uint8_t *command);
static void handle_room_creation(const int32_t socket, const uint8_t *command);
static void handle_room_removal(const int32_t socket, const uint8_t *command);
static void handle_new_room_name(const int32_t socket, const uint8_t *command);
static void handle_new_room_desc(const int32_t socket, const uint8_t *command);
static void handle_room_exit_toggling(const int32_t socket, const uint8_t *command);
static void prepare_for_new_room_desc(const int32_t socket, const uint8_t *command);
static void prepare_for_new_room_name(const int32_t socket, const uint8_t *command);
static void prepare_for_room_mk(const int32_t socket, const uint8_t *command);
static void prepare_for_room_rm(const int32_t socket);

#define not_waiting_for_player_response get_player_holding_for_input(socket) == false
#define exit_if_change_not_confirmed \
	if (tolower(command[0]) != 'y') {\
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);\
		set_player_wait_state(socket, NO_WAIT_STATE);\
		set_player_holding_for_input(socket, 0);\
		return;\
	}
#define exit_if_dir_not_valid \
	if ((ensure_player_moving_valid_dir(socket, command)) == EXIT_FAILURE) {\
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);\
		set_player_wait_state(socket, NO_WAIT_STATE);\
		set_player_holding_for_input(socket, 0);\
		return;\
	}
#define reset_player_wait_state(x) \
	set_player_wait_state(socket, NO_WAIT_STATE);\
	set_player_holding_for_input(socket, 0);\

int32_t interpret_command(const size_t socket)
{
	uint8_t *command = process_buffer(socket);

	if (not_waiting_for_player_response) {
		struct Command *info = get_command_info(command);
		free(command);

		if (info->type == COMMAND_NOT) {
			print_to_player(socket, INVALCMD);
			free(info);
			return EXIT_FAILURE;
		}

		return do_cmd_action(socket, info);
	}

	// should probably handle 'quit' if they want to exit this process, or C-C
	switch (get_player_wait_state(socket)) {
	case THEIR_NAME:
		handle_incoming_name(socket, command);
		break;
	case THEIR_PASSWORD_EXISTING:
		handle_existing_pass(socket, command);
		break;
	case THEIR_PASSWORD_NEWPRELIM:
		set_player_confirm_new_pw(socket, command);
		break;
	case THEIR_PASSWORD_NEWFINAL:
		handle_new_pass(socket, command);
		break;

	case WAIT_ENTER_NEW_ROOM_NAME:
		prepare_for_new_room_name(socket, command);
		break;
	case WAIT_CONFIRM_NEW_ROOM_NAME:
		handle_new_room_name(socket, command);
		break;

	case WAIT_ENTER_NEW_ROOM_DESC:
		prepare_for_new_room_desc(socket, command);
		break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		handle_new_room_desc(socket, command);
		break;

	case WAIT_ROOM_REMOVAL_CHECK:
		prepare_for_room_rm(socket);
		break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		handle_room_removal(socket, command);
		break;

	case WAIT_ROOM_CREATION_DIR:
		prepare_for_room_mk(socket, command);
		break;
	case WAIT_ROOM_CREATION_CONF:
		handle_room_creation(socket, command);
		break;

	// TODO
	case WAIT_ENTER_FLAG_NAME:
		// CHECK_IF_IS_VALID_FLAG;
		// finish
		set_player_wait_state(socket, NO_WAIT_STATE);
		set_player_holding_for_input(socket, 0);
		break;
	// TODO
	case WAIT_ENTER_EXIT_NAME:
		handle_room_exit_toggling(socket, command);
		break;

	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(socket), get_player_name(socket));
		free(command);
		return EXIT_FAILURE;
	}

	free(command);
	return EXIT_SUCCESS;
}

static void handle_room_exit_toggling(const int32_t socket, const uint8_t *command)
{
	exit_if_dir_not_valid;

	int32_t rv;
	struct Coordinates coords = calc_coords_from_playerloc_and_dir(socket);
	struct RoomRecord *map = lookup_room(coords);

	if (map != NULL) {
		rv = adjust_room_exit(socket, coords);
		if (rv == EXIT_SUCCESS) {
			print_to_player(socket, PRINT_TOGGLED_ROOM_EXIT);
		} else if (rv == EXIT_FAILURE) {
			print_to_player(socket, PRINT_COULDNT_TOGGLE_EXIT);
		} else if (rv == 2) {
			//print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
		}

		free_room(map);
	} else {
		print_to_player(socket, PRINT_COULDNT_EXIT_NO_ROOM);
	}

	clear_player_store(socket);
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
}

static void handle_new_room_desc(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;

	int32_t rv = adjust_room_desc(socket);
	if (rv == 1) {
		print_to_player(socket, PRINT_ADJUSTMENT_SUCCESSFUL);
	} else if (rv == -2) {
		print_to_player(socket, PRINT_COULDNT_ADJUST_ROOM);
	} else if (rv == -3) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	clear_player_store(socket);
	print_to_player(socket, SHOWROOM);
}

static void handle_new_room_name(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;
	// TODO: add the commented out stuff
	//void *loc_in_echo = NULL;

	int32_t rv = adjust_room_name(socket);
	if (rv == 0) {
		print_to_player(socket, PRINT_ADJUSTMENT_SUCCESSFUL);
	} else if (rv == 1) {
		print_to_player(socket, PRINT_COULDNT_ADJUST_ROOM);
	} else if (rv == 2) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	clear_player_store(socket);

	// TODO: rework
	/*
	print_to_player(socket, SHOWROOM);
	uint8_t room_echo[BUFFER_LENGTH] = {0};
	loc_in_echo = mempcpy(room_echo, get_player_name(socket), strlen((char *)get_player_name(socket)));
	loc_in_echo = mempcpy(loc_in_echo, " changes the room name.", 23);
	print_not_player(socket, room_echo, ROOM_ONLY);
	*/
}

static void handle_room_creation(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;

	struct Coordinates coords = calc_coords_from_playerloc_and_dir(socket);

	struct RoomRecord *room = lookup_room(coords); 
	_Bool room_exists = room->found;
	free(room);
	if (room_exists == true)
		goto exists_already;

	// check here for their perms
	// print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);

	struct NewRoom rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.coords.x = coords.x;
	rconfig.coords.y = coords.y;
	rconfig.coords.z = coords.z;
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.owner = get_player_name(socket);
	rconfig.flags = (uint8_t*)"none";

	int32_t rv = insert_room(rconfig);
	if (rv == 0) {
		print_to_player(socket, PRINT_ROOM_CREATION_SUCCESS);
		rv = adjust_room_exit(socket, coords);
		print_to_player(socket, SHOWROOM);
	} else {
		print_to_player(socket, PRINT_ROOM_CREATION_FAILURE);
	}

	exists_already:
		print_to_player(socket, PRINT_ROOM_ALREADY_EXISTS);

	clear_player_store(socket);
	reset_player_wait_state(socket);
}

static void handle_room_removal(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;

	// TODO: check exits etc handled & players in room moved
	int32_t rv = remove_room(socket);
	if (rv == 0) {
		print_to_player(socket, PRINT_ROOM_REMOVAL_SUCCESS);
	} else if (rv == 1) {
		print_to_player(socket, PRINT_ROOM_REMOVAL_FAILURE);
	} else if (rv == 2) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	clear_player_store(socket);
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
}

static uint8_t *process_buffer(const size_t socket)
{
	size_t len = strlen((char *)get_player_buffer(socket));
	
	if (get_player_wait_state(socket) == THEIR_NAME)
		len = NAMES_MAX;

	uint8_t *command = calloc(len, sizeof(uint8_t));

	for (size_t i = 0; i < len; ++i) {
		command[i] = get_player_buffer(socket)[i];
		if (command[i] == '\0')
			break;

		if (get_player_wait_state(socket) != THEIR_NAME) {
			if (isupper(command[i]) != 0) 
				command[i] = tolower(command[i]);

			if (isspace(command[i]) != 0) {
				command[i] = '\0';
				break;
			}
		}
	}

	return command;
}

static _Bool handle_incoming_name(const int32_t socket, const uint8_t *command)
{
	if (check_if_name_is_valid(socket, command) == false) 
		return false;

	if (check_if_name_is_reserved(socket, command) == true)
		return false;

	if (check_if_player_is_already_online(socket) == true)
		return false;

	set_player_name(socket, command);

	struct PlayerDBRecord *player;
	if ((player = lookup_player(get_player_name(socket))) != NULL) {
		print_to_player(socket, REQUEST_PW_FOR_EXISTING);
		set_player_wait_state(socket, THEIR_PASSWORD_EXISTING);
		set_player_name(socket, command);
	} else {
		print_to_player(socket, REQUEST_PW_FOR_NEW);
		set_player_wait_state(socket, THEIR_PASSWORD_NEWPRELIM);
		set_player_name(socket, command);
	}

	free(player);

	return true;
}

// TODO: use enums
static void prepare_for_new_room_desc(const int32_t socket, const uint8_t *command)
{
	init_player_store(socket);
	set_player_store_replace(socket, command);
	set_player_wait_state(socket, WAIT_CONFIRM_NEW_ROOM_DESC);
	print_to_player(socket, PRINT_CONFIRM_NEW_ROOM_DESC);
}

// TODO: use enums
static void prepare_for_new_room_name(const int32_t socket, const uint8_t *command)
{
	init_player_store(socket);
	set_player_store_replace(socket, command);
	set_player_wait_state(socket, WAIT_CONFIRM_NEW_ROOM_NAME);
	print_to_player(socket, PRINT_CONFIRM_NEW_ROOM_NAME);
}

// TODO: use enums
static void prepare_for_room_mk(const int32_t socket, const uint8_t *command)
{
	exit_if_dir_not_valid;

	init_player_store(socket);
	set_player_store_replace(socket, command);
	set_player_store_append(socket, (uint8_t*)"\0");
	set_player_wait_state(socket, WAIT_ROOM_CREATION_CONF);
	print_to_player(socket, PRINT_ROOM_CREATION_CONFIRMALL);
}

static void prepare_for_room_rm(const int32_t socket)
{
	print_to_player(socket, PRINT_ROOM_REMOVAL_CONFIRM);
	set_player_wait_state(socket, WAIT_ROOM_REMOVAL_CONFIRM);
}
