#include "command_interpretation_handler.h"

static uint8_t *process_buffer(const size_t socket);
static _Bool handle_incoming_name(const int32_t socket, const uint8_t *command);
static void handle_room_creation(const int32_t socket, const uint8_t *command);

#define player_is_not_quitting memcmp(command, "quit", 4) != 0 

int32_t interpret_command(const size_t socket)
{
	uint8_t *command = {0};
	int32_t rv = 0;
	int32_t x, y, z;
	void *loc_in_echo = NULL;

	command = process_buffer(socket);

	if (get_player_holding_for_input(socket) == false && player_is_not_quitting) {
		if (strlen((char*)command) > get_max_command_len() || check_clist(socket, command) == EXIT_FAILURE) { 
			print_to_player(socket, INVALCMD);
			return EXIT_FAILURE;
		}
	}

	// should probably handle 'quit' if they want to exit this process
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
		if (command[0] == 'y' || command[0] == 'Y') {
			rv = adjust_room_details(ADJUSTING_ROOM_NAME, -1, socket,
				get_player_coord(X_COORD_REQUEST, socket),
				get_player_coord(Y_COORD_REQUEST, socket),
				get_player_coord(Z_COORD_REQUEST, socket));
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
		loc_in_echo = mempcpy(room_echo, get_player_name(socket), BUFFER_LENGTH);
		loc_in_echo = mempcpy(loc_in_echo, " changes the room name.", BUFFER_LENGTH - strlen((char*)room_echo));
		print_not_player(socket, room_echo, ROOM_ONLY);
		break;
	case WAIT_ENTER_NEW_ROOM_DESC:
		init_player_store(socket);
		set_player_wait_state(socket, WAIT_CONFIRM_NEW_ROOM_DESC);
		set_player_store_replace(socket, get_player_buffer(socket));
		print_to_player(socket, PRINT_CONFIRM_NEW_ROOM_DESC);
		break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		if (command[0] == 'y' || command[0] == 'Y') {
			rv = adjust_room_details(ADJUSTING_ROOM_DESC, -1, socket,
				get_player_coord(X_COORD_REQUEST, socket),
				get_player_coord(Y_COORD_REQUEST, socket),
				get_player_coord(Z_COORD_REQUEST, socket));
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
		break;

	case WAIT_ROOM_REMOVAL_CHECK:
		if (command[0] == 'y' || command[0] == 'Y') {
			print_to_player(socket, PRINT_ROOM_REMOVAL_CONFIRM);
			set_player_wait_state(socket, WAIT_ROOM_REMOVAL_CONFIRM);
		} else {
			print_to_player(socket, PRINT_EXITING_CMD_WAIT);
			set_player_wait_state(socket, NO_WAIT_STATE);
			set_player_holding_for_input(socket, 0);
		}
		break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		if (command[0] == 'y' || command[0] == 'Y') {
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
		if ((ensure_player_moving_valid_dir(socket, command)) == EXIT_FAILURE)
			break;

		init_player_store(socket);
		set_player_store_replace(socket, command);
		x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, socket);
		y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, socket);
		z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, socket);
		struct Map *map = lookup_room(x, y, z, -1);

		if (map != NULL) {
			rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, socket, x, y, z);
			x = get_player_coord(X_COORD_REQUEST, socket);
			y = get_player_coord(Y_COORD_REQUEST, socket);
			z = get_player_coord(Z_COORD_REQUEST, socket);
			rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, socket, x, y, z);
			print_to_player(socket, PRINT_TOGGLED_ROOM_EXIT);
			free_room(map);
		} else if (rv == 0) {
			print_to_player(socket, PRINT_COULDNT_TOGGLE_EXIT);
		} else if (rv == -1) {
			print_to_player(socket, PRINT_COULDNT_EXIT_NO_ROOM);
		}

		clear_player_store(socket);
		set_player_wait_state(socket, NO_WAIT_STATE);
		set_player_holding_for_input(socket, 0);
		break;

	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(socket), get_player_name(socket));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void handle_room_creation(const int32_t socket, const uint8_t *command)
{
	if (command[0] != 'y' && command[0] != 'Y') {
		print_to_player(socket, PRINT_EXITING_CMD_WAIT);
	}
	int32_t x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, socket);
	int32_t y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, socket);
	int32_t z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, socket);

	struct NewRoom rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.x = x;
	rconfig.y = y;
	rconfig.z = z;
	rconfig.desc = (uint8_t*)"There's nothing here but a lack of oxygen and the sense of impending doom.";
	rconfig.owner = get_player_name(socket);
	rconfig.flags = (uint8_t*)"none";
	int rv = insert_room(rconfig);

	if (rv == 1) {
		print_to_player(socket, PRINT_ROOM_CREATION_SUCCESS);
		rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, socket, 
			get_player_coord(X_COORD_REQUEST, socket),
			get_player_coord(Y_COORD_REQUEST, socket),
			get_player_coord(Z_COORD_REQUEST, socket));
		rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, socket, x, y, z);
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

	if (lookup_player(get_player_name(socket)) == true) {
		print_to_player(socket, REQUEST_PW_FOR_EXISTING);
		set_player_wait_state(socket, THEIR_PASSWORD_EXISTING);
	} else {
		print_to_player(socket, REQUEST_PW_FOR_NEW);
		set_player_wait_state(socket, THEIR_PASSWORD_NEWPRELIM);
	}

	return true;
}
