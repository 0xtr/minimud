#include "command_interpretation_handler.h"

static uint8_t *process_command_from_pbuf(const size_t pnum);
static _Bool handle_incoming_name(const int32_t pnum, const uint8_t *command);
static void handle_room_creation(const int32_t pnum, const uint8_t *command);

// TODO: split
int32_t interpret_command(const size_t pnum)
{
	uint8_t *command;
	int32_t rv = 0;
	int32_t x, y, z;

	// clean up
	command = process_command_from_pbuf(pnum);
	if (get_player_holding_for_input(pnum) == 1 && strcmp((char*)command, "quit") != 0) {
		if (strlen((char*)command) > get_max_command_len() || 
		   ((check_clist(pnum, command)) != 1)) { // not a valid command, inform the player 
			print_to_player(pnum, INVALCMD);
			return EXIT_FAILURE;
		}
	}

	// should probably handle 'quit' if they want to exit this process
	switch (get_player_wait_state(pnum)) {
	case THEIR_NAME:
		handle_incoming_name(pnum, command);
		break;
	case THEIR_PASSWORD_EXISTING:
		handle_existing_pass(pnum, command);
		break;
	case THEIR_PASSWORD_NEWPRELIM:
		set_player_confirm_new_pw(pnum, command);
		break;
	case THEIR_PASSWORD_NEWFINAL:
		handle_new_pass(pnum, command);
		break;
	case WAIT_ENTER_NEW_ROOM_NAME:
		init_player_store(pnum);
		set_player_wait_state (pnum, WAIT_CONFIRM_NEW_ROOM_NAME);
		print_to_player(pnum, PRINT_CONFIRM_NEW_ROOM_NAME);
		break;
	case WAIT_CONFIRM_NEW_ROOM_NAME:
		if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
			rv = adjust_room_details(ADJUSTING_ROOM_NAME, -1, pnum,
				get_player_coord(X_COORD_REQUEST, pnum),
				get_player_coord(Y_COORD_REQUEST, pnum),
				get_player_coord(Z_COORD_REQUEST, pnum));
			if (rv == 1) {
				print_to_player(pnum, PRINT_ADJUSTMENT_SUCCESSFUL);
			} else if (rv == -2) {
				print_to_player(pnum, PRINT_COULDNT_ADJUST_ROOM);
			} else if (rv == -3) {
				print_to_player(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
			}
		} else {
			print_to_player(pnum, PRINT_EXITING_CMD_WAIT);
		}
		set_player_wait_state(pnum, NO_WAIT_STATE);
		set_player_holding_for_input(pnum, 0);
		clear_player_store(pnum);

		print_to_player(pnum, SHOWROOM);
		uint8_t room_echo[BUFFER_LENGTH] = {0};
		strncpy((char*)room_echo, (char*)get_player_name(pnum), BUFFER_LENGTH);
		strncat((char*)room_echo, " changes the room name.", BUFFER_LENGTH - strlen((char*)room_echo));
		print_not_player(pnum, room_echo, ROOM_ONLY);
		break;
	case WAIT_ENTER_NEW_ROOM_DESC:
		init_player_store(pnum);
		set_player_wait_state(pnum, WAIT_CONFIRM_NEW_ROOM_DESC);
		set_player_store_replace(pnum, get_player_buffer(pnum));
		print_to_player(pnum, PRINT_CONFIRM_NEW_ROOM_DESC);
		break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		if ((strcmp((char*)command, "y") == 0) || (strcmp((char*)command, "Y") == 0)) {
			rv = adjust_room_details(ADJUSTING_ROOM_DESC, -1, pnum,
				get_player_coord(X_COORD_REQUEST, pnum),
				get_player_coord(Y_COORD_REQUEST, pnum),
				get_player_coord(Z_COORD_REQUEST, pnum));
			if (rv == 1) {
				print_to_player(pnum, PRINT_ADJUSTMENT_SUCCESSFUL);
			} else if (rv == -2) {
				print_to_player(pnum, PRINT_COULDNT_ADJUST_ROOM);
			} else if (rv == -3) {
				print_to_player(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
			}
		} else {
			print_to_player(pnum, PRINT_EXITING_CMD_WAIT);
		}
		set_player_wait_state(pnum, NO_WAIT_STATE);
		set_player_holding_for_input(pnum, 0);
		clear_player_store(pnum);
		print_to_player(pnum, SHOWROOM);
		break;

	case WAIT_ROOM_REMOVAL_CHECK:
		if (command[0] == 'y' || command[0] == 'Y') {
			print_to_player(pnum, PRINT_ROOM_REMOVAL_CONFIRM);
			set_player_wait_state(pnum, WAIT_ROOM_REMOVAL_CONFIRM);
		} else {
			print_to_player(pnum, PRINT_EXITING_CMD_WAIT);
			set_player_wait_state(pnum, NO_WAIT_STATE);
			set_player_holding_for_input(pnum, 0);
		}
		break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		if (command[0] == 'y' || command[0] == 'Y') {
			rv = remove_room(pnum);
			if (rv == 1) {
				print_to_player(pnum, PRINT_ROOM_REMOVAL_SUCCESS);
			} else if (rv == -2) {
				print_to_player(pnum, PRINT_ROOM_REMOVAL_FAILURE);
			} else if (rv == -3) {
				print_to_player(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
			}
		} else {
			print_to_player(pnum, PRINT_EXITING_CMD_WAIT);
		}
		set_player_wait_state(pnum, NO_WAIT_STATE);
		set_player_holding_for_input(pnum, 0);
		break;
	case WAIT_ROOM_CREATION_DIR:
		if ((ensure_player_moving_valid_dir(pnum, command)) == EXIT_FAILURE)
			break;
		init_player_store(pnum);
		set_player_store_replace(pnum, command);
		set_player_store_append(pnum, (uint8_t*)"\0");
		set_player_wait_state(pnum, WAIT_ROOM_CREATION_CONF);
		print_to_player(pnum, PRINT_ROOM_CREATION_CONFIRMALL);
		break;
	case WAIT_ROOM_CREATION_CONF:
		handle_room_creation(pnum, command);
		break;

	case WAIT_ENTER_FLAG_NAME:
		// CHECK_IF_IS_VALID_FLAG;
		// finish
		set_player_wait_state(pnum, NO_WAIT_STATE);
		set_player_holding_for_input(pnum, 0);
		break;
	case WAIT_ENTER_EXIT_NAME:
		if ((ensure_player_moving_valid_dir(pnum, command)) == EXIT_FAILURE)
			break;

		init_player_store(pnum);
		set_player_store_replace(pnum, command);
		x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum);
		y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum);
		z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum);
		struct Map *map = lookup_room(x, y, z, -1);

		if (map != NULL) {
			rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
			x = get_player_coord(X_COORD_REQUEST, pnum);
			y = get_player_coord(Y_COORD_REQUEST, pnum);
			z = get_player_coord(Z_COORD_REQUEST, pnum);
			rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, x, y, z);
			print_to_player(pnum, PRINT_TOGGLED_ROOM_EXIT);
			free_room(map);
		} else if (rv == 0) {
			print_to_player(pnum, PRINT_COULDNT_TOGGLE_EXIT);
		} else if (rv == -1) {
			print_to_player(pnum, PRINT_COULDNT_EXIT_NO_ROOM);
		}

		clear_player_store(pnum);
		set_player_wait_state(pnum, NO_WAIT_STATE);
		set_player_holding_for_input(pnum, 0);
		break;

	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(pnum), get_player_name(pnum));
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

static void handle_room_creation(const int32_t pnum, const uint8_t *command)
{
	if ((strcmp((char*)command, "y") != 0) && (strcmp((char*)command, "Y") != 0)) {
		print_to_player(pnum, PRINT_EXITING_CMD_WAIT);
	}
	int32_t x = calc_coord_from_playerloc_and_dir(X_COORD_REQUEST, pnum);
	int32_t y = calc_coord_from_playerloc_and_dir(Y_COORD_REQUEST, pnum);
	int32_t z = calc_coord_from_playerloc_and_dir(Z_COORD_REQUEST, pnum);

	struct NewRoom rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.x = x;
	rconfig.y = y;
	rconfig.z = z;
	rconfig.desc = (uint8_t*)"There's nothing here but a lack of oxygen and the sense of impending doom.";
	rconfig.owner = get_player_name(pnum);
	rconfig.flags = (uint8_t*)"none";
	int rv = insert_room(rconfig);

	if (rv == 1) {
		print_to_player(pnum, PRINT_ROOM_CREATION_SUCCESS);
		rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 0, pnum, 
			get_player_coord(X_COORD_REQUEST, pnum),
			get_player_coord(Y_COORD_REQUEST, pnum),
			get_player_coord(Z_COORD_REQUEST, pnum));
		rv = adjust_room_details(ADJUSTING_ROOM_EXIT, 1, pnum, x, y, z);
		print_to_player(pnum, SHOWROOM);
	} else if (rv == -3) {
		print_to_player(pnum, PRINT_INSUFFICIENT_PERMISSIONS);
	} else {
		print_to_player(pnum, PRINT_ROOM_CREATION_FAILURE);
	}

	clear_player_store(pnum);
	set_player_wait_state(pnum, NO_WAIT_STATE);
	set_player_holding_for_input(pnum, 0);
}

static uint8_t *process_command_from_pbuf(const size_t pnum)
{
 	uint8_t *command = calloc(get_max_command_len(), sizeof(uint8_t));

	for (size_t i = 0; i < get_max_command_len(); ++i) {
		command[i] = get_player_buffer(pnum)[i];
		if (isspace(get_player_buffer(pnum)[i]) != 0) {
			command[i] = '\0';
			break;
		}
	}
	return command;
}

static _Bool handle_incoming_name(const int32_t pnum, const uint8_t *command)
{
	if (check_if_name_is_valid(pnum, command)) 
		return false;
	if (check_if_name_is_reserved(pnum, command))
		return false;
	if (check_if_player_is_already_online(pnum))
		return false;

	set_player_name(pnum, command);
	if (lookup_player(get_player_name(pnum)) == true) {
		print_to_player(pnum, REQUEST_PW_FOR_EXISTING);
		set_player_wait_state(pnum, THEIR_PASSWORD_EXISTING);
	} else {
		print_to_player(pnum, REQUEST_PW_FOR_NEW);
		set_player_wait_state(pnum, THEIR_PASSWORD_NEWPRELIM);
	}

	return true;
}
