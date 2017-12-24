#include "interpret_command.h"

static uint8_t *process_buffer(struct player_live_record *player);
static _Bool handle_incoming_name(struct player_live_record *player, const uint8_t *command);
static void handle_room_creation(struct player_live_record *player, const uint8_t *command);
static void handle_room_removal(struct player_live_record *player, const uint8_t *command);
static void alter_room_name(struct player_live_record *player, const uint8_t *command);
static void alter_room_desc(struct player_live_record *player, const uint8_t *command);
static void alter_room_links(struct player_live_record *player, const uint8_t *command);
static void prepare_for_new_room_desc(struct player_live_record *player, const uint8_t *command);
static void prepare_for_new_room_name(struct player_live_record *player, const uint8_t *command);
static void prepare_for_room_mk(struct player_live_record *player, const uint8_t *command);
static void prepare_for_room_rm(struct player_live_record *player);

#define not_waiting_for_player_response \
	player->holding_for_input == false

int32_t interpret_command(struct player_live_record *player)
{
	uint8_t *command = process_buffer(player);
	struct command *info = get_command_info(command);

	if (not_waiting_for_player_response) {
		free(command);

		if (info->type == COMMAND_NOT) {
			print_to_player(player, INVALCMD);
			free(info);

			return EXIT_FAILURE;
		}

		return do_cmd_action(player, info);
	}

	// should probably handle 'quit' if they want to exit this process, or C-C
	switch (player->wait_state) {
	case THEIR_NAME:
		handle_incoming_name(player, command); break;
	case THEIR_PASSWORD_EXISTING:
		handle_existing_pass(player, command); break;
	case THEIR_PASSWORD_NEWPRELIM:
		set_player_confirm_new_pw(player, command); break;
	case THEIR_PASSWORD_NEWFINAL:
		handle_new_pass(player, command); break;

	case WAIT_ENTER_NEW_ROOM_NAME:
		prepare_for_new_room_name(player, command); break;
	case WAIT_CONFIRM_NEW_ROOM_NAME:
		alter_room_name(player, command); break;

	case WAIT_ENTER_NEW_ROOM_DESC:
		prepare_for_new_room_desc(player, command); break;
	case WAIT_CONFIRM_NEW_ROOM_DESC:
		alter_room_desc(player, command); break;

	case WAIT_ROOM_REMOVAL_CHECK:
		prepare_for_room_rm(player); break;
	case WAIT_ROOM_REMOVAL_CONFIRM:
		handle_room_removal(player, command); break;

	case WAIT_ROOM_CREATION_DIR:
		prepare_for_room_mk(player, command); break;
	case WAIT_ROOM_CREATION_CONF:
		handle_room_creation(player, command); break;

	// TODO
	case WAIT_ENTER_FLAG_NAME:
		// CHECK_IF_IS_VALID_FLAG;
		// finish
		reset_player_state(player);
		break;
	// TODO
	case WAIT_ENTER_EXIT_NAME:
		alter_room_links(player, command); break;
	default:
		fprintf(stdout, "Unhandled wait state %d on player %s.\n", 
				player->wait_state, player->name);
	}

	free(command);
	free(info);
	return EXIT_SUCCESS;
}

static void alter_room_links(struct player_live_record *player, const uint8_t *command)
{
	if ((ensure_player_moving_valid_dir(player, command)) == EXIT_FAILURE) {
		reset_player_state(player);
		return;
	}

	int32_t rv;
	struct coordinates player_coords = get_player_coords(player);
	struct coordinates dest_coords = calc_coords_from_playerloc_and_dir(player);
	struct room_db_record *src_room = lookup_room(player_coords);
	struct room_db_record *dest_room = lookup_room(dest_coords);

	if (dest_room == NULL) {
		print_to_player(player, PRINT_COULDNT_EXIT_NO_ROOM);
		goto failed;
	}

	if (compare_room_owner(player, player_coords) == EXIT_FAILURE) {
		print_to_player(player, PRINT_INSUFFICIENT_PERMISSIONS);
		goto failed;
	}

	struct command *info = get_command_info(command);
	const int32_t dir = info->subtype;

	rv = link_rooms(dir, src_room, dest_room);

	if (rv == EXIT_SUCCESS) {
		print_to_player(player, PRINT_TOGGLED_ROOM_EXIT);
	} else if (rv == EXIT_FAILURE) {
		print_to_player(player, PRINT_COULDNT_TOGGLE_EXIT);
	}

	failed:
	
	if (src_room != NULL) 
		free(src_room);
	if (dest_room != NULL) 
		free(dest_room);

	reset_player_state(player);
}

static void alter_room_desc(struct player_live_record *player, const uint8_t *command)
{
	if (command != NULL && tolower(command[0]) != 'y') {
		print_to_player(player, PRINT_EXITING_CMD_WAIT);
		reset_player_state(player);
	}

	int32_t rv = adjust_room_desc(player);

	if (rv == 0) {
		print_to_player(player, PRINT_ADJUSTMENT_SUCCESSFUL);
	} else if (rv == 1) {
		print_to_player(player, PRINT_COULDNT_ADJUST_ROOM);
	} else if (rv == 2) {
		print_to_player(player, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	reset_player_state(player);

	struct room_db_record *room = lookup_room(get_player_coords(player));

	print_room_to_player(player, room);

	free(room);
}

static void alter_room_name(struct player_live_record *player, const uint8_t *command)
{
	if (command != NULL && tolower(command[0]) != 'y') {
		print_to_player(player, PRINT_EXITING_CMD_WAIT);
		reset_player_state(player);
	}

	int32_t rv = adjust_room_name(player);

	if (rv == 0) {
		print_to_player(player, PRINT_ADJUSTMENT_SUCCESSFUL);
	} else if (rv == 1) {
		print_to_player(player, PRINT_COULDNT_ADJUST_ROOM);
	} else if (rv == 2) {
		print_to_player(player, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	reset_player_state(player);

	struct room_db_record *room = lookup_room(get_player_coords(player));

	print_room_to_player(player, room);

	free(room);
}

static void handle_room_creation(struct player_live_record *player, const uint8_t *command)
{
	if (command != NULL && tolower(command[0]) != 'y') {
		print_to_player(player, PRINT_EXITING_CMD_WAIT);
		reset_player_state(player);
	}

	struct coordinates dest_coords = calc_coords_from_playerloc_and_dir(player);
	struct room_db_record *room = lookup_room(dest_coords); 
	const int32_t room_id = room->id;

	free(room);

	if (room_id > 0) {
		print_to_player(player, PRINT_ROOM_ALREADY_EXISTS);
		reset_player_state(player);
		return;
	}

	// check here for their perms
	// print_to_player(player, PRINT_INSUFFICIENT_PERMISSIONS);

	struct room_blueprint rconfig;
	rconfig.name = (uint8_t*)"NULL SPACE";
	rconfig.coords = dest_coords;
	rconfig.desc = (uint8_t*)"It is pitch black. You are likely to be eaten by a null character.";
	rconfig.owner = player->name;
	rconfig.flags = (uint8_t*)"none";

	struct room_db_record *existing = lookup_room(get_player_coords(player));
	struct room_db_record *new = insert_room(rconfig);

	if (new == NULL) {
		print_to_player(player, PRINT_ROOM_CREATION_FAILURE);
		goto done;
	}

	print_to_player(player, PRINT_ROOM_CREATION_SUCCESS);

	struct command *info = get_command_info(player->store);
	const int32_t dir = info->subtype;
	free(info);

	link_rooms(dir, existing, new);

	free(existing); // get the updated room image with exits
	existing = lookup_room(get_player_coords(player));

	print_room_to_player(player, existing);

	done:

	if (new != NULL)
		free(new);
	if (existing != NULL)
		free(existing);

	reset_player_state(player);
}

static void handle_room_removal(struct player_live_record *player, const uint8_t *command)
{
	if (command != NULL && tolower(command[0]) != 'y') {
		print_to_player(player, PRINT_EXITING_CMD_WAIT);
		reset_player_state(player);
	}

	// TODO: check exits etc handled & players in room moved
	int32_t rv = remove_room(player);
	if (rv == 0) {
		print_to_player(player, PRINT_ROOM_REMOVAL_SUCCESS);
	} else if (rv == -1) {
		print_to_player(player, PRINT_ROOM_REMOVAL_FAILURE);
	} else if (rv == -2) {
		print_to_player(player, PRINT_INSUFFICIENT_PERMISSIONS);
	}

	reset_player_state(player);
}

static uint8_t *process_buffer(struct player_live_record *player)
{
	size_t len = strlen((char *)player->buffer);
	
	if (player->wait_state == THEIR_NAME)
		len = NAMES_MAX;

	uint8_t *command = calloc(len+1, sizeof(uint8_t));

	for (size_t i = 0; i < len; ++i) {
		command[i] = player->buffer[i];
		if (command[i] == '\0')
			break;

		if (player->wait_state != THEIR_NAME) {
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

static _Bool handle_incoming_name(struct player_live_record *player, const uint8_t *command)
{
	if (check_if_name_is_valid(player, command) == false) 
		return false;
	if (check_if_name_is_reserved(player, command) == true)
		return false;
	if (check_if_player_is_already_online(player, command) == true)
		return false;

	set_player_name(player, command);
	struct player_db_record *plookup = lookup_player(player->name);

	if (plookup != NULL) {
		print_to_player(player, REQUEST_PW_FOR_EXISTING);
		player->wait_state = THEIR_PASSWORD_EXISTING;
	} else {
		print_to_player(player, REQUEST_PW_FOR_NEW);
		player->wait_state = THEIR_PASSWORD_NEWPRELIM;
	}

	free(plookup);

	return true;
}

// TODO: use enums
static void prepare_for_new_room_desc(struct player_live_record *player, const uint8_t *command)
{
	init_player_store(player);
	set_player_store_replace(player, command);

	player->wait_state = WAIT_CONFIRM_NEW_ROOM_DESC;
	print_to_player(player, PRINT_CONFIRM_NEW_ROOM_DESC);
}

// TODO: use enums
static void prepare_for_new_room_name(struct player_live_record *player, const uint8_t *command)
{
	init_player_store(player);
	set_player_store_replace(player, command);

	player->wait_state = WAIT_CONFIRM_NEW_ROOM_NAME;
	print_to_player(player, PRINT_CONFIRM_NEW_ROOM_NAME);
}

// TODO: use enums
static void prepare_for_room_mk(struct player_live_record *player, const uint8_t *command)
{
	if ((ensure_player_moving_valid_dir(player, command)) == EXIT_FAILURE) {
		reset_player_state(player);
		return;
	}

	init_player_store(player);

	set_player_store_replace(player, command);
	set_player_store_append(player, (uint8_t*)"\0");
	player->wait_state = WAIT_ROOM_CREATION_CONF;

	print_to_player(player, PRINT_ROOM_CREATION_CONFIRMALL);
}

static void prepare_for_room_rm(struct player_live_record *player)
{
	print_to_player(player, PRINT_ROOM_REMOVAL_CONFIRM);
	player->wait_state = WAIT_ROOM_REMOVAL_CONFIRM;
}
