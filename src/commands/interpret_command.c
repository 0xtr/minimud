#include "interpret_command.h"

static uint8_t *process_buffer(const size_t socket);
static _Bool handle_incoming_name(const int32_t socket, const uint8_t *command);
static void handle_room_creation(const int32_t socket, const uint8_t *command);
static void handle_new_room_name(const int32_t socket, const uint8_t *command);
static void handle_new_room_desc(const int32_t socket, const uint8_t *command);
static void handle_room_exit_toggling(const int32_t socket, const uint8_t *command);

#define player_is_not_quitting memcmp(command, "quit", 4) != 0 
#define player_confirms command[0] == 'y' || command[0] == 'Y'
#define not_expecting_info_from_player get_player_holding_for_input(socket) == false
#define command_is_longer_than_max strlen((char*)command) > get_max_command_len() 
#define not_a_valid_cmd check_cmd_against_list(command)

int32_t interpret_command(const size_t socket)
{
	uint8_t *command = {0};
	int32_t rv = 0;

	command = process_buffer(socket);

	if (not_expecting_info_from_player && player_is_not_quitting) {
		if (command_is_longer_than_max || not_a_valid_cmd) {
			print_to_player(socket, INVALCMD);
			return EXIT_FAILURE;
		}
	}

	if (get_player_wait_state(socket) == NO_WAIT_STATE)
		return execute_player_command(socket, command);

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
		init_player_store(socket);
		set_player_wait_state (socket, WAIT_CONFIRM_NEW_ROOM_NAME);
		print_to_player(socket, PRINT_CONFIRM_NEW_ROOM_NAME);
		break;
	case WAIT_CONFIRM_NEW_ROOM_NAME:
		handle_new_room_name(socket, command);
		break;
	case WAIT_ENTER_NEW_ROOM_DESC:
		init_player_store(socket);
		set_player_wait_state(socket, WAIT_CONFIRM_NEW_ROOM_DESC);
		set_player_store_replace(socket, get_player_buffer(socket));
		print_to_player(socket, PRINT_CONFIRM_NEW_ROOM_DESC);
		break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		handle_new_room_desc(socket, command);
		break;

	case WAIT_ROOM_REMOVAL_CHECK:
		if (player_confirms) {
			print_to_player(socket, PRINT_ROOM_REMOVAL_CONFIRM);
			set_player_wait_state(socket, WAIT_ROOM_REMOVAL_CONFIRM);
		} else {
			print_to_player(socket, PRINT_EXITING_CMD_WAIT);
			set_player_wait_state(socket, NO_WAIT_STATE);
			set_player_holding_for_input(socket, 0);
		}
		break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		if (player_confirms) {
			rv = remove_room(socket);
			if (rv == 1) {
				print_to_player(socket, PRINT_ROOM_REMOVAL_SUCCESS);
			} else if (rv == -2) {
				print_to_player(socket, PRINT_ROOM_REMOVAL_FAILURE);
			} else if (rv == -3) {
				print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
			}
		} else {
			print_to_player(socket, PRINT_EXITING_CMD_WAIT);
		}
		set_player_wait_state(socket, NO_WAIT_STATE);
		set_player_holding_for_input(socket, 0);
		break;
	case WAIT_ROOM_CREATION_DIR:
		if ((ensure_player_moving_valid_dir(socket, command)) == EXIT_FAILURE)
			break;
		init_player_store(socket);
		set_player_store_replace(socket, command);
		set_player_store_append(socket, (uint8_t*)"\0");
		set_player_wait_state(socket, WAIT_ROOM_CREATION_CONF);
		print_to_player(socket, PRINT_ROOM_CREATION_CONFIRMALL);
		break;
	case WAIT_ROOM_CREATION_CONF:
		handle_room_creation(socket, command);
		break;

	case WAIT_ENTER_FLAG_NAME:
		// CHECK_IF_IS_VALID_FLAG;
		// finish
		set_player_wait_state(socket, NO_WAIT_STATE);
		set_player_holding_for_input(socket, 0);
		break;
	case WAIT_ENTER_EXIT_NAME:
		handle_room_exit_toggling(socket, command);
		break;

	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(socket), get_player_name(socket));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void handle_room_exit_toggling(const int32_t socket, const uint8_t *command)
{
	int32_t rv;
	if ((ensure_player_moving_valid_dir(socket, command)) == EXIT_FAILURE)
		return;

	init_player_store(socket);
	set_player_store_replace(socket, command);

	struct Coordinates coords = calc_coords_from_playerloc_and_dir(socket);
	struct RoomRecord *map = lookup_room(coords);

	if (map != NULL) {
		rv = adjust_room_exit(socket, get_player_coords(socket), coords);
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
	struct Coordinates coords = get_player_coords(socket);
	int32_t rv;

	if (player_confirms) {
		rv = adjust_room_details(ADJUSTING_ROOM_DESC, socket, coords);
		if (rv == 1) {
			print_to_player(socket, PRINT_ADJUSTMENT_SUCCESSFUL);
		} else if (rv == -2) {
			print_to_player(socket, PRINT_COULDNT_ADJUST_ROOM);
		} else if (rv == -3) {
			print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
		}
	} else {
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);
	}
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	clear_player_store(socket);
	print_to_player(socket, SHOWROOM);
}

static void handle_new_room_name(const int32_t socket, const uint8_t *command)
{
	int32_t rv;
	void *loc_in_echo = NULL;

	if (player_confirms) {
		rv = adjust_room_details(ADJUSTING_ROOM_NAME, socket, get_player_coords(socket));

		if (rv == 1) {
			print_to_player(socket, PRINT_ADJUSTMENT_SUCCESSFUL);
		} else if (rv == -2) {
			print_to_player(socket, PRINT_COULDNT_ADJUST_ROOM);
		} else if (rv == -3) {
			print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
		}
	} else {
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);
	}

	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	clear_player_store(socket);

	print_to_player(socket, SHOWROOM);
	uint8_t room_echo[BUFFER_LENGTH] = {0};
	loc_in_echo = mempcpy(room_echo, get_player_name(socket), strlen((char *)get_player_name(socket)));
	loc_in_echo = mempcpy(loc_in_echo, " changes the room name.", 23);
	print_not_player(socket, room_echo, ROOM_ONLY);
}

static void handle_room_creation(const int32_t socket, const uint8_t *command)
{
	if (command[0] != 'y' && command[0] != 'Y')
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);

	struct Coordinates coords = calc_coords_from_playerloc_and_dir(socket);

	struct NewRoom rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.coords.x = coords.x;
	rconfig.coords.y = coords.y;
	rconfig.coords.z = coords.z;
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.owner = get_player_name(socket);
	rconfig.flags = (uint8_t*)"none";
	int32_t rv = insert_room(rconfig);

	if (rv == 1) {
		print_to_player(socket, PRINT_ROOM_CREATION_SUCCESS);
		rv = adjust_room_exit(socket, coords, get_player_coords(socket));
		rv = adjust_room_exit(socket, get_player_coords(socket), coords);
		print_to_player(socket, SHOWROOM);
	} else if (rv == -3) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
	} else {
		print_to_player(socket, PRINT_ROOM_CREATION_FAILURE);
	}

	clear_player_store(socket);
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
}

static uint8_t *process_buffer(const size_t socket)
{
	size_t len = get_max_command_len();
	
	if (get_player_wait_state(socket) == THEIR_NAME)
		len = NAMES_MAX;

	uint8_t *command = calloc(len, sizeof(uint8_t));

	for (size_t i = 0; i < len; ++i) {
		command[i] = get_player_buffer(socket)[i];

		if (command[i] == '\0')
			break;

		if (get_player_wait_state(socket) != THEIR_NAME) {
			if (isspace(get_player_buffer(socket)[i]) != 0) {
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
