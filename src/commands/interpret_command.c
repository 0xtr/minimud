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
	struct command *info = get_command_info(command);

	if (not_waiting_for_player_response) {
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
		handle_incoming_name(socket, command); break;
	case THEIR_PASSWORD_EXISTING:
		handle_existing_pass(socket, command); break;
	case THEIR_PASSWORD_NEWPRELIM:
		set_player_confirm_new_pw(socket, command); break;
	case THEIR_PASSWORD_NEWFINAL:
		handle_new_pass(socket, command); break;

	case WAIT_ENTER_NEW_ROOM_NAME:
		prepare_for_new_room_name(socket, command); break;
	case WAIT_CONFIRM_NEW_ROOM_NAME:
		handle_new_room_name(socket, command); break;

	case WAIT_ENTER_NEW_ROOM_DESC:
		prepare_for_new_room_desc(socket, command); break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		handle_new_room_desc(socket, command); break;

	case WAIT_ROOM_REMOVAL_CHECK:
		prepare_for_room_rm(socket); break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		handle_room_removal(socket, command); break;

	case WAIT_ROOM_CREATION_DIR:
		prepare_for_room_mk(socket, command); break;
	case WAIT_ROOM_CREATION_CONF:
		handle_room_creation(socket, command); break;

	// TODO
	case WAIT_ENTER_FLAG_NAME:
		// CHECK_IF_IS_VALID_FLAG;
		// finish
		set_player_wait_state(socket, NO_WAIT_STATE);
		set_player_holding_for_input(socket, 0);
		break;
	// TODO
	case WAIT_ENTER_EXIT_NAME:
		handle_room_exit_toggling(socket, command); break;
	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", get_player_wait_state(socket), get_player_name(socket));
	}

	free(command);
	free(info);
	return EXIT_SUCCESS;
}

static void handle_room_exit_toggling(const int32_t socket, const uint8_t *command)
{
	exit_if_dir_not_valid;

	int32_t rv;
	struct coordinates player_coords = get_player_coords(socket);
	struct coordinates dest_coords = calc_coords_from_playerloc_and_dir(socket);
	struct room_atom *src_room = lookup_room(player_coords);
	struct room_atom *dest_room = lookup_room(dest_coords);

	if (dest_room == NULL) {
		print_to_player(socket, PRINT_COULDNT_EXIT_NO_ROOM);
		goto failed;
	}

	if (compare_room_owner(socket, player_coords) == EXIT_FAILURE) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
		goto failed;
	}

	struct command *info = get_command_info(command);
	const int32_t dir = info->subtype;

	rv = adjust_room_exit(dir, src_room, dest_room);

	if (rv == EXIT_SUCCESS) {
		print_to_player(socket, PRINT_TOGGLED_ROOM_EXIT);
	} else if (rv == EXIT_FAILURE) {
		print_to_player(socket, PRINT_COULDNT_TOGGLE_EXIT);
	}

	failed:
	
	if (src_room != NULL) 
		free(src_room);
	if (dest_room != NULL) 
		free(dest_room);
	reset_player_state(socket);
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

	reset_player_state(socket);
	print_to_player(socket, SHOWROOM);
}

static void handle_new_room_name(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;

	int32_t rv = adjust_room_name(socket);
	if (rv == 0) {
		print_to_player(socket, PRINT_ADJUSTMENT_SUCCESSFUL);
	} else if (rv == 1) {
		print_to_player(socket, PRINT_COULDNT_ADJUST_ROOM);
	} else if (rv == 2) {
		print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	reset_player_state(socket);
	print_to_player(socket, SHOWROOM);
}

static void handle_room_creation(const int32_t socket, const uint8_t *command)
{
	exit_if_change_not_confirmed;

	struct coordinates dest_coords = calc_coords_from_playerloc_and_dir(socket);
	struct room_atom *room = lookup_room(dest_coords); 
	const int32_t room_id = room->id;

	free(room);

	if (room_id > 0) {
		print_to_player(socket, PRINT_ROOM_ALREADY_EXISTS);
		reset_player_state(socket);
		return;
	}

	// check here for their perms
	// print_to_player(socket, PRINT_INSUFFICIENT_PERMISSIONS);

	struct room_blueprint rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.coords = dest_coords;
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.owner = get_player_name(socket);
	rconfig.flags = (uint8_t*)"none";

	struct room_atom *existing = lookup_room(get_player_coords(socket));
	struct room_atom *new = insert_room(rconfig);

	if (new == NULL) {
		print_to_player(socket, PRINT_ROOM_CREATION_FAILURE);
		goto done;
	}

	print_to_player(socket, PRINT_ROOM_CREATION_SUCCESS);

	struct command *info = get_command_info(get_player_store(socket));
	const int32_t dir = info->subtype;
	free(info);

	adjust_room_exit(dir, existing, new);

	print_to_player(socket, SHOWROOM);

	done:

	free(new);
	free(existing);
	reset_player_state(socket);
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

	reset_player_state(socket);
}

static uint8_t *process_buffer(const size_t socket)
{
	size_t len = strlen((char *)get_player_buffer(socket));
	
	if (get_player_wait_state(socket) == THEIR_NAME)
		len = NAMES_MAX;

	uint8_t *command = calloc(len+1, sizeof(uint8_t));

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

	struct PlayerDBRecord *player = lookup_player(get_player_name(socket));

	if (player != NULL) {
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
