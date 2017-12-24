#include "printing.h"

static int32_t print_to_other_players(struct player_live_record *player, const uint8_t *buffer);
static _Bool set_buffer_for_movement(struct player_live_record *player, const int32_t argument);
static _Bool print_all_commands(struct player_live_record *player);
static _Bool is_in_same_room(struct player_live_record *player, struct coordinates room_coords);
static void append_coordinates_for_printing(struct player_live_record *player,
		const struct coordinates coords);

int32_t print_to_player(struct player_live_record *player, const int32_t argument)
{
	#define IS_DIRECTION_ARG (argument >= 0 && argument <= 19)

	switch (argument) {
	case INVALCMD:
		set_player_buffer_replace(player, "Invalid command. Type \'commands\'.\n");
		break;
	case SHOWCMDS:
		print_all_commands(player);
		return EXIT_SUCCESS;
	case INVALDIR:
		set_player_buffer_replace(player, "Cannot move in that direction. Type 'look' to view room.\n");
		break;
	case REQUEST_PW_FOR_NEW:
		set_player_buffer_replace(player, "You've provided the name [");
		set_player_buffer_append(player, player->name);
		set_player_buffer_append(player, "].\n\n");
		set_player_buffer_append(player, "Provide a password less than ");
		set_player_buffer_append(player, BUFFER_LENGTH_STR);
		set_player_buffer_append(player, " characters long.\n\n");
		break;
	case REQUEST_PW_CONFIRM:
		set_player_buffer_replace(player, "Confirm your password by typing it out once more.\n");
		break;
	case REQUEST_PW_FOR_EXISTING:
		set_player_buffer_replace(player, "Provide your password.\n");
		break;
	case ATTEMPT_CREATE_USR:
		set_player_buffer_replace(player, "Attempting to create your character...\n");
		break;
	case MISMATCH_PW_SET:
		set_player_buffer_replace(player, "Password mismatch. Start over.\nWhat is your NAME.\n");
		break;
	case PLAYER_CREATION_FAILED:
		set_player_buffer_replace(player, "Character creation failed. You should never see this.\n");
		break;
	case PLAYER_CREATION_SUCCESS:
		set_player_buffer_replace(player, "Character created. Entering the world...\n");
		break;
	case PLAYER_ALREADY_ONLINE: 
		set_player_buffer_replace(player, "That player is already connected.\n");
		break;
	case INCORRECT_PASSWORD:
		set_player_buffer_replace(player, "Incorrect password. Restarting.\n\nWhat is your NAME.\n");
		break;
	case UNABLE_TO_RETRIEVE_CHAR:
		set_player_buffer_replace(player, "Couldn't retrieve your character.\n");
		break;
	case NAME_UNAVAILABLE:
		set_player_buffer_replace(player, "That name is unavailable.\n");
		break;
	case ALPHANUM_NAMES_ONLY:
		set_player_buffer_replace(player, "Only alphanumeric characters are permitted.\n");
		break;
	case NAME_NOT_WITHIN_PARAMS:
		set_player_buffer_replace(player, "Provide an alphanumeric NAME that is at least ");
		set_player_buffer_append(player, NAMES_MIN_STR);
		set_player_buffer_append(player, " characters long, and no more than ");
		set_player_buffer_append(player, NAMES_MAX_STR);
		set_player_buffer_append(player, " characters. Try again.\n\nWhat is your NAME.\n");
		break;
	case PRINT_PROVIDE_NEW_ROOM_NAME:
		set_player_buffer_replace(player, "Enter a new room name, of up to ");
		set_player_buffer_append(player, MAX_ROOM_NAME_STR);
		set_player_buffer_append(player, " chars.\n");
		break;
	case PRINT_PROVIDE_NEW_ROOM_DESC:
		set_player_buffer_replace(player, "Enter a new room description, of up to ");
		set_player_buffer_append(player, BUFFER_LENGTH_STR);
		set_player_buffer_append(player, " chars.\n");
		break;
	case PRINT_CONFIRM_NEW_ROOM_DESC:
		set_player_buffer_replace(player, "Confirm the new description by typing Y/y. You entered:\n");
		set_player_buffer_append(player, player->store);
		set_player_buffer_append(player, "\nIf this is wrong, type something other than Y/y.\n\n");
		break;
	case PRINT_CONFIRM_NEW_ROOM_NAME:
		set_player_buffer_replace(player, "Confirm the new name by typing Y/y. You entered:\n");
		set_player_buffer_append(player, player->store);
		set_player_buffer_append(player, "\nIf this is wrong, type something other than Y/y.\n\n");
		break;
	case PRINT_ADJUSTMENT_SUCCESSFUL:
		set_player_buffer_replace(player, "Room adjusted successfully. Exiting editor.\n");
		break;
	case PRINT_COULDNT_ADJUST_ROOM:
		set_player_buffer_replace(player, "An error occurred. Room adjustment failed. Exiting editor.\n");
		break;
	case PRINT_EXITING_CMD_WAIT:
		set_player_buffer_replace(player, "Exiting editor - returning you to the (real) world.\n");
		break;
	case PRINT_INSUFFICIENT_PERMISSIONS:
		set_player_buffer_replace(player, "You don't have permission to do that.\n");
		break;
	case PRINT_ROOM_EXIT_CHANGED:
		set_player_buffer_replace(player, "Room exit changed.\n");
		break;
	case PRINT_ROOM_FLAG_CHANGED:
		set_player_buffer_replace(player, "Room flag toggled.\n");
		break;
	case PRINT_ROOM_REMOVAL_CHECK:
		set_player_buffer_replace(player, "You're trying to delete this room. Are you sure you want to do this?\nType only y/Y to confirm.\n\n");
		break;
	case PRINT_ROOM_REMOVAL_CONFIRM:
		set_player_buffer_replace(player, "Again, confirm that you want to delete your CURRENT ROOM.\n");
		break;
	case PRINT_ROOM_REMOVAL_SUCCESS:
		set_player_buffer_replace(player, "Room removed successfully.\n");
		break;
	case PRINT_ROOM_REMOVAL_FAILURE:
		set_player_buffer_replace(player, "Room removal failed - you are not the owner of this room.\n");
		break;
	case PRINT_ROOM_CREATION_GIVE_DIR:
		set_player_buffer_replace(player, "Which direction are you trying to create a room in? (from current)\n");
		break;
	case PRINT_ROOM_CREATION_CONFIRMALL:
		set_player_buffer_replace(player, "You're adding a room in the direction of: ");
		set_player_buffer_append(player, player->store);
		set_player_buffer_append(player, ". Confirm this by typing y/Y, or decline by typing anything else.\n");
		break;
	case PRINT_ROOM_CREATION_CANNOT:
		set_player_buffer_replace(player, "Room creation couldn't be completed right now.\n");
		break;
	case PRINT_ROOM_CREATION_SUCCESS:
		set_player_buffer_replace(player, "Room creation complete.\n");
		break;
	case PRINT_ROOM_ALREADY_EXISTS:
		set_player_buffer_replace(player, "A room already exists in that direction. Exiting editor.\n");
		break;
	case PRINT_ROOM_CREATION_FAILURE:
		set_player_buffer_replace(player, "Creation failed. Contact an administrator.\n");
		break;
	case PRINT_REMOVED_FROM_ROOM:
		set_player_buffer_replace(player, "You've been moved from your previous room; the owner may have deleted it.\nMoved to:\n\n");
		break;
	case PRINT_PROVIDE_ROOM_EXIT_NAME:
		set_player_buffer_replace(player, "Which direction are you trying to toggle exit visibility for?\n");
		break;
	case PRINT_PROVIDE_ROOM_FLAG_NAME:
		set_player_buffer_replace(player, "Which flag are you trying to toggle?\n");
		break;
	case PRINT_TOGGLED_ROOM_EXIT:
		set_player_buffer_replace(player, "Room exit visibility toggled. Exiting editor.\n");
		break;
	case PRINT_COULDNT_TOGGLE_EXIT:
		set_player_buffer_replace(player, "Unable to toggle the exit for that room.\n");
		break;
	case PRINT_COULDNT_EXIT_NO_ROOM:
		set_player_buffer_replace(player, "There's no room in that direction.\n");
		break;
	default:
		if (IS_DIRECTION_ARG)
			set_buffer_for_movement(player, argument);
	}

	assert(outgoing_handler(player) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

static _Bool set_buffer_for_movement (struct player_live_record *player, const int32_t argument) {
	switch (argument) {
	case 0:
		set_player_buffer_replace(player, "You move NORTH.\n");
		return EXIT_SUCCESS;
	case 2:
		set_player_buffer_replace(player, "You move EAST.\n");
		return EXIT_SUCCESS;
	case 4:
		set_player_buffer_replace(player, "You move SOUTH.\n");
		return EXIT_SUCCESS;
	case 6:
		set_player_buffer_replace(player, "You move WEST.\n");
		return EXIT_SUCCESS;
	case 8:
		set_player_buffer_replace(player, "You move UP.\n");
		return EXIT_SUCCESS;
	case 10:
		set_player_buffer_replace(player, "You move DOWN.\n");
		return EXIT_SUCCESS;
	case 12:
		set_player_buffer_replace(player, "You move NORTHEAST.\n");
		return EXIT_SUCCESS;
	case 14:
		set_player_buffer_replace(player, "You move SOUTHEAST.\n");
		return EXIT_SUCCESS;
	case 16:
		set_player_buffer_replace(player, "You move SOUTHWEST.\n");
		return EXIT_SUCCESS;
	case 18:
		set_player_buffer_replace(player, "You move NORTHWEST.\n");
		return EXIT_SUCCESS;
	default:
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}

static _Bool is_in_same_room(struct player_live_record *player, struct coordinates room_coords)
{
	struct coordinates coords = get_player_coords(player);

	return coords.x == room_coords.x && coords.y == room_coords.y && 
		coords.z == room_coords.z;
}

int32_t print_room_to_player(struct player_live_record *player, struct room_db_record *room)
{
	struct coordinates coords = get_player_coords(player);

	set_player_buffer_replace(player, 
			(room->found == false) ? (void *)"NULL SPACE" : (void *)room->rname);

	append_coordinates_for_printing(player, coords);
	assert(outgoing_handler(player) == EXIT_SUCCESS);

	set_player_buffer_replace(player, "Exits:");

	if (room->exits[NORTH_EXIT] >= 0)
		set_player_buffer_append(player, " N");
	if (room->exits[SOUTH_EXIT] >= 0)
		set_player_buffer_append(player, " S");
	if (room->exits[EAST_EXIT] >= 0)
		set_player_buffer_append(player, " E");
	if (room->exits[WEST_EXIT] >= 0)
		set_player_buffer_append(player, " W");
	if (room->exits[UP_EXIT] >= 0)
		set_player_buffer_append(player, " U");
	if (room->exits[DOWN_EXIT] >= 0)
		set_player_buffer_append(player, " D");
	if (room->exits[NORTHEAST_EXIT] >= 0)
		set_player_buffer_append(player, " NE");
	if (room->exits[SOUTHEAST_EXIT] >= 0)
		set_player_buffer_append(player, " SE");
	if (room->exits[SOUTHWEST_EXIT] >= 0)
		set_player_buffer_append(player, " SW");
	if (room->exits[NORTHWEST_EXIT] >= 0)
		set_player_buffer_append(player, " NW");

	if (strlen((char *)player->buffer) == 6)
		set_player_buffer_append(player, " NONE");

	assert(outgoing_handler(player) == EXIT_SUCCESS);

	set_player_buffer_replace(player, 
			(room ->found == false) ?
			(void *)"It is pitch black. You are "
			"likely to be eaten by a null character."
			: (void *)room->rdesc);

	if (room->rdesc != NULL && strlen((char *)room->rdesc) > 0)
		set_player_buffer_append(player, "\n");
	assert(outgoing_handler(player) == EXIT_SUCCESS);

	const uint8_t *target = player->name;

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct player_live_record *each = get_player_by_index(i);

		if (each->name == target)
			continue;

		if (is_in_same_room(each, coords) == true) {
			set_player_buffer_replace(player, each->name);
			set_player_buffer_append(player, " is here too.\n");
			assert(outgoing_handler(player) == EXIT_SUCCESS);
		}
	}

	return EXIT_SUCCESS;
}

static void append_coordinates_for_printing(struct player_live_record *player, const struct coordinates coords)
{
	uint8_t param_x[sizeof(coords.x)] = {0};
	uint8_t param_y[sizeof(coords.y)] = {0};
	uint8_t param_z[sizeof(coords.z)] = {0};
	snprintf((char *)param_x, sizeof(coords.x), "%d", coords.x);
	snprintf((char *)param_y, sizeof(coords.y), "%d", coords.y);
	snprintf((char *)param_z, sizeof(coords.z), "%d", coords.z);

	set_player_buffer_append(player, " [");
	set_player_buffer_append(player, param_x);
	set_player_buffer_append(player, "][");
	set_player_buffer_append(player, param_y);
	set_player_buffer_append(player, "][");
	set_player_buffer_append(player, param_z);
	set_player_buffer_append(player, "]");
}

static _Bool print_all_commands(struct player_live_record *player)
{
	int32_t commands_on_line = 0;
	size_t player_buffer_len;
	void *cmd;

	set_player_buffer_replace(player, "Available commands:\n");

	for (size_t i = 0; i < get_num_of_available_cmds(); ++i) {
		cmd = get_command(i);
		player_buffer_len = strlen((char *)player->buffer);

		if (strlen((char *)player->buffer) + strlen((char *)cmd) > BUFFER_LENGTH) {
			assert(outgoing_handler(player) == EXIT_SUCCESS);
			player_buffer_len = 0;
		}

		if (player_buffer_len == 0) {
			set_player_buffer_replace(player, cmd);
		} else {
			set_player_buffer_append(player, cmd);
		}

		if (++commands_on_line < 5) {
			set_player_buffer_append(player, "\t");
			if (strlen((char *)cmd) < 7)
				set_player_buffer_append(player, "\t");
		}

		if (commands_on_line == 5) {
			commands_on_line = 0;
			assert(outgoing_handler(player) == EXIT_SUCCESS);
		}
	}

	if (strlen((char *)player->buffer) != 0)
		set_player_buffer_append(player, "\n");

	assert(outgoing_handler(player) == EXIT_SUCCESS);

	return true;
}

int32_t greet_player(const int32_t socket)
{
	struct player_live_record *player = get_player(socket);
	set_player_buffer_replace(player, "WELCOME.\n\n");
	set_player_buffer_append(player, "Please provide a NAME; this can be two words and up to ");
	set_player_buffer_append(player, NAMES_MAX_STR);
	set_player_buffer_append(player, " characters long in total.\n\nIf you've "
			"already created a character, enter your previous name to resume.\n\n");

	assert(outgoing_handler(player) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

int32_t print_player_speech(struct player_live_record *player)
{
	#define SKIP_SAY_TOKEN 4 // length req'd for the actual say command + the space after that
	uint8_t *buffer = calloc(BUFFER_LENGTH+1, sizeof(uint8_t));
	void *loc_in_buf = NULL;

	loc_in_buf = mempcpy(buffer, player->name, strlen((char *)player->name));
	loc_in_buf = mempcpy(loc_in_buf, " says: ", 7);

	int32_t to_add = strlen((char *)&player->buffer[SKIP_SAY_TOKEN]);
	if (to_add + strlen((char *)buffer) > BUFFER_LENGTH)
		to_add = BUFFER_LENGTH - strlen((char *)buffer);

	loc_in_buf = mempcpy(loc_in_buf, &player->buffer[SKIP_SAY_TOKEN], to_add);

	print_to_other_players(player, buffer);

#ifdef DEBUG
	printf("print_player_speech to others: %s\n", buffer);
#endif

	free(buffer);

	uint8_t substr[to_add];
	memset(substr, '\0', to_add);

	memcpy(substr, "You say: ", 9);
	memcpy(&substr[9], &player->buffer[SKIP_SAY_TOKEN], to_add);
	memcpy(&substr[9+to_add], "\n", 1);

	set_player_buffer_replace(player, substr);

#ifdef DEBUG
	printf("print_player_speech to them: %s (%lu)\n", player->buffer, strlen((char *)player->buffer));
#endif

	assert(outgoing_handler(player) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

static int32_t print_to_other_players(struct player_live_record *player, const uint8_t *buffer)
{
	struct coordinates coords = get_player_coords(player);

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct player_live_record *present = get_player_by_index(i);

		printf("show it to player %s?\n", present->name);

		if (present->socket_num == player->socket_num)
			continue;

		if (is_in_same_room(present, coords) == true) {
			set_player_buffer_replace(present, buffer);
			set_player_buffer_append(present, "\n");
			assert(outgoing_handler(present) == EXIT_SUCCESS);
		}
	}

	return EXIT_SUCCESS;
}
