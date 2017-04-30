#include "printing.h"

static int32_t print_to_other_players(const int32_t socket, const uint8_t *buffer);
static _Bool set_buffer_for_movement(const int32_t socket, const int32_t argument);
static _Bool print_all_commands(const int32_t socket);
static _Bool build_room_image(const int32_t socket);
static _Bool is_in_same_room(const int32_t socket, struct coordinates room_coords);
static void append_coordinates_for_printing(const int32_t socket, const struct coordinates coords);

int32_t print_to_player(const int32_t socket, const int32_t argument)
{
	#define IS_DIRECTION_ARG (argument >= 0 && argument <= 19)

	switch (argument) {
	case INVALCMD:
		set_player_buffer_replace(socket, "Invalid command. Type \'commands\'.\n");
		break;
	case SHOWCMDS:
		print_all_commands(socket);
		return EXIT_SUCCESS;
	case SHOWROOM:
		build_room_image(socket);
		return EXIT_SUCCESS;
	case INVALDIR:
		set_player_buffer_replace(socket, "Cannot move in that direction. Type 'look' to view room.\n");
		break;
	case REQUEST_PW_FOR_NEW:
		set_player_buffer_replace(socket, "You've provided the name [");
		set_player_buffer_append(socket, get_player_name(socket));
		set_player_buffer_append(socket, "].\n\n");
		set_player_buffer_append(socket, "Please provide a password less than ");
		set_player_buffer_append(socket, BUFFER_LENGTH_STR);
		set_player_buffer_append(socket, " characters long.\n\n");
		break;
	case REQUEST_PW_CONFIRM:
		set_player_buffer_replace(socket, "Please confirm your password by typing it out once more.\n");
		break;
	case REQUEST_PW_FOR_EXISTING:
		set_player_buffer_replace(socket, "Please provide your password.\n");
		break;
	case ATTEMPT_CREATE_USR:
		set_player_buffer_replace(socket, "Attempting to create your character...\n");
		break;
	case MISMATCH_PW_SET:
		set_player_buffer_replace(socket, "Password mismatch. Start over.\nPlease provide a NAME.\n");
		break;
	case PLAYER_CREATION_FAILED:
		set_player_buffer_replace(socket, "Character creation failed. You should never see this.\n");
		break;
	case PLAYER_CREATION_SUCCESS:
		set_player_buffer_replace(socket, "Character created successfully. Entering the world...\n");
		break;
	case PLAYER_ALREADY_ONLINE: 
		set_player_buffer_replace(socket, "That player is already connected. Please provide another name.\n");
		break;
	case INCORRECT_PASSWORD:
		set_player_buffer_replace(socket, "Incorrect password. Restarting.\nPlease provide a NAME.\n");
		break;
	case UNABLE_TO_RETRIEVE_CHAR:
		set_player_buffer_replace(socket, "Couldn't retrieve your character.\n");
		break;
	case NAME_UNAVAILABLE:
		set_player_buffer_replace(socket, "That name is unavailable.\n");
		break;
	case ALPHANUM_NAMES_ONLY:
		set_player_buffer_replace(socket, "Only alphanumeric characters are permitted.\n");
		break;
	case NAME_NOT_WITHIN_PARAMS:
		set_player_buffer_replace(socket, "Provide an alphanumeric NAME that is at least ");
		set_player_buffer_append(socket, NAMES_MIN_STR);
		set_player_buffer_append(socket, " characters long, and no more than ");
		set_player_buffer_append(socket, NAMES_MAX_STR);
		set_player_buffer_append(socket, " characters. Try again.\n\nPlease provide a NAME.\n");
		break;
	case PRINT_PROVIDE_NEW_ROOM_NAME:
		set_player_buffer_replace(socket, "Enter a new room name, of up to ");
		set_player_buffer_append(socket, MAX_ROOM_NAME_STR);
		set_player_buffer_append(socket, " chars.\n");
		break;
	case PRINT_PROVIDE_NEW_ROOM_DESC:
		set_player_buffer_replace(socket, "Enter a new room description, of up to ");
		set_player_buffer_append(socket, BUFFER_LENGTH_STR);
		set_player_buffer_append(socket, " chars.\n");
		break;
	case PRINT_CONFIRM_NEW_ROOM_DESC:
		set_player_buffer_replace(socket, "Confirm the new description by typing Y/y. You entered:");
		set_player_buffer_append(socket, get_player_store(socket));
		set_player_buffer_replace(socket, "\nIf this is wrong, type something other than Y/y.\n");
		break;
	case PRINT_CONFIRM_NEW_ROOM_NAME:
		set_player_buffer_replace(socket, "Confirm the new name by typing Y/y. You entered:");
		set_player_buffer_append(socket, get_player_store(socket));
		set_player_buffer_replace(socket, "\nIf this is wrong, type something other than Y/y.\n");
		break;
	case PRINT_ADJUSTMENT_SUCCESSFUL:
		set_player_buffer_replace(socket, "Room adjusted successfully. Exiting editor.\n");
		break;
	case PRINT_COULDNT_ADJUST_ROOM:
		set_player_buffer_replace(socket, "Some kind of error occurred. Room adjustment failed. Exiting editor.\n");
		break;
	case PRINT_EXITING_CMD_WAIT:
		set_player_buffer_replace(socket, "Exiting editor - returning you to the (real) world.\n");
		break;
	case PRINT_INSUFFICIENT_PERMISSIONS:
		set_player_buffer_replace(socket, "You don't have permission to do that.\n");
		break;
	case PRINT_ROOM_EXIT_CHANGED:
		set_player_buffer_replace(socket, "Room exit changed.\n");
		break;
	case PRINT_ROOM_FLAG_CHANGED:
		set_player_buffer_replace(socket, "Room flag toggled.\n");
		break;
	case PRINT_ROOM_REMOVAL_CHECK:
		set_player_buffer_replace(socket, "You're trying to delete this room. Are you sure you want to do this?\nType only y/Y to confirm.\n\n");
		break;
	case PRINT_ROOM_REMOVAL_CONFIRM:
		set_player_buffer_replace(socket, "Again, confirm that you want to delete your CURRENT ROOM.\n");
		break;
	case PRINT_ROOM_REMOVAL_SUCCESS:
		set_player_buffer_replace(socket, "Room removed successfully.\n");
		break;
	case PRINT_ROOM_REMOVAL_FAILURE:
		set_player_buffer_replace(socket, "Room removal failed - you are not the owner of this room.\n");
		break;
	case PRINT_ROOM_CREATION_GIVE_DIR:
		set_player_buffer_replace(socket, "Which direction are you trying to create a room in? (from current)\n");
		break;
	case PRINT_ROOM_CREATION_CONFIRMALL:
		set_player_buffer_replace(socket, "You're adding a room in the direction of: ");
		set_player_buffer_append(socket, get_player_store(socket));
		set_player_buffer_append(socket, ". Confirm this by typing y/Y, or decline by typing anything else.\n");
		break;
	case PRINT_ROOM_CREATION_CANNOT:
		set_player_buffer_replace(socket, "Room creation couldn't be completed right now.\n");
		break;
	case PRINT_ROOM_CREATION_SUCCESS:
		set_player_buffer_replace(socket, "Room creation complete.\n");
		break;
	case PRINT_ROOM_ALREADY_EXISTS:
		set_player_buffer_replace(socket, "A room already exists in that direction. Exiting editor.\n");
		break;
	case PRINT_ROOM_CREATION_FAILURE:
		set_player_buffer_replace(socket, "Creation failed. Contact an administrator.\n");
		break;
	case PRINT_REMOVED_FROM_ROOM:
		set_player_buffer_replace(socket, "You've been moved from your previous room; the owner may have deleted it.\nMoved to:\n\n");
		break;
	case PRINT_PROVIDE_ROOM_EXIT_NAME:
		set_player_buffer_replace(socket, "Which direction are you trying to toggle exit visibility for?\n");
		break;
	case PRINT_PROVIDE_ROOM_FLAG_NAME:
		set_player_buffer_replace(socket, "Which flag are you trying to toggle?\n");
		break;
	case PRINT_TOGGLED_ROOM_EXIT:
		set_player_buffer_replace(socket, "Room exit visibility toggled. Exiting editor.\n");
		break;
	case PRINT_COULDNT_TOGGLE_EXIT:
		set_player_buffer_replace(socket, "Unable to toggle the exit for that room.\n");
		break;
	case PRINT_COULDNT_EXIT_NO_ROOM:
		set_player_buffer_replace(socket, "There's no room in that direction.\n");
		break;
	default:
		if (IS_DIRECTION_ARG)
			set_buffer_for_movement(socket, argument);
	}

	assert(outgoing_handler(socket) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

static _Bool set_buffer_for_movement (const int32_t socket, const int32_t argument) {
	switch (argument) {
	case 0:
		set_player_buffer_replace(socket, "You move NORTH.\n");
		return EXIT_SUCCESS;
	case 2:
		set_player_buffer_replace(socket, "You move EAST.\n");
		return EXIT_SUCCESS;
	case 4:
		set_player_buffer_replace(socket, "You move SOUTH.\n");
		return EXIT_SUCCESS;
	case 6:
		set_player_buffer_replace(socket, "You move WEST.\n");
		return EXIT_SUCCESS;
	case 8:
		set_player_buffer_replace(socket, "You move UP.\n");
		return EXIT_SUCCESS;
	case 10:
		set_player_buffer_replace(socket, "You move DOWN.\n");
		return EXIT_SUCCESS;
	case 12:
		set_player_buffer_replace(socket, "You move NORTHEAST.\n");
		return EXIT_SUCCESS;
	case 14:
		set_player_buffer_replace(socket, "You move SOUTHEAST.\n");
		return EXIT_SUCCESS;
	case 16:
		set_player_buffer_replace(socket, "You move SOUTHWEST.\n");
		return EXIT_SUCCESS;
	case 18:
		set_player_buffer_replace(socket, "You move NORTHWEST.\n");
		return EXIT_SUCCESS;
	default:
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}

static _Bool is_in_same_room(const int32_t socket, struct coordinates room_coords)
{
	struct coordinates coords = get_player_coords(socket);

	return coords.x == room_coords.x && coords.y == room_coords.y && 
		coords.z == room_coords.z;
}

static _Bool build_room_image (const int32_t socket) {
	struct coordinates coords = get_player_coords(socket);
	struct room_atom *map = lookup_room(coords);

	set_player_buffer_replace(socket, 
			(map->found == false) ? (void *)"NULL SPACE" : (void *)map->rname);
	append_coordinates_for_printing(socket, coords);
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	set_player_buffer_replace(socket, "Exits:");

	if (map->exits[NORTH_EXIT] >= 0)
		set_player_buffer_append(socket, " N");
	if (map->exits[SOUTH_EXIT] >= 0)
		set_player_buffer_append(socket, " S");
	if (map->exits[EAST_EXIT] >= 0)
		set_player_buffer_append(socket, " E");
	if (map->exits[WEST_EXIT] >= 0)
		set_player_buffer_append(socket, " W");
	if (map->exits[UP_EXIT] >= 0)
		set_player_buffer_append(socket, " U");
	if (map->exits[DOWN_EXIT] >= 0)
		set_player_buffer_append(socket, " D");
	if (map->exits[NORTHEAST_EXIT] >= 0)
		set_player_buffer_append(socket, " NE");
	if (map->exits[SOUTHEAST_EXIT] >= 0)
		set_player_buffer_append(socket, " SE");
	if (map->exits[SOUTHWEST_EXIT] >= 0)
		set_player_buffer_append(socket, " SW");
	if (map->exits[NORTHWEST_EXIT] >= 0)
		set_player_buffer_append(socket, " NW");

	if (strlen((char *)get_player_buffer(socket)) == 6)
		set_player_buffer_append(socket, " NONE");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	set_player_buffer_replace(socket, 
			(map ->found == false) ?
			(void *)"It is pitch black. You are "
			"likely to be eaten by a null character."
			: (void *)map->rdesc);
	set_player_buffer_append(socket, "\n");
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	const uint8_t *target = get_player_name(socket);

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct player_live_record *each = get_player(i);

		if (each->name == target)
			continue;

		if (is_in_same_room(each->socket_num, coords) == true) {
			set_player_buffer_replace(socket, each->name);
			set_player_buffer_append(socket, " is here too.\n");
			assert(outgoing_handler(socket) == EXIT_SUCCESS);
		}
	}

	free(map);

	return true;
}

static void append_coordinates_for_printing(const int32_t socket, const struct coordinates coords)
{
	uint8_t param_x[sizeof(coords.x)] = {0};
	uint8_t param_y[sizeof(coords.y)] = {0};
	uint8_t param_z[sizeof(coords.z)] = {0};
	snprintf((char *)param_x, sizeof(coords.x), "%d", coords.x);
	snprintf((char *)param_y, sizeof(coords.y), "%d", coords.y);
	snprintf((char *)param_z, sizeof(coords.z), "%d", coords.z);

	set_player_buffer_append(socket, " [");
	set_player_buffer_append(socket, param_x);
	set_player_buffer_append(socket, "][");
	set_player_buffer_append(socket, param_y);
	set_player_buffer_append(socket, "][");
	set_player_buffer_append(socket, param_z);
	set_player_buffer_append(socket, "]");
}

static _Bool print_all_commands(const int32_t socket)
{
	int32_t commands_on_line = 0;
	size_t player_buffer_len;
	void *cmd;

	set_player_buffer_replace(socket, "Available commands:\n");

	for (size_t i = 0; i != get_num_of_available_cmds(); ++i) {
		cmd = get_command(i);
		player_buffer_len = strlen((char *)get_player_buffer(socket));

		if (strlen((char *)get_player_buffer(socket)) + strlen((char *)cmd) > BUFFER_LENGTH) {
			assert(outgoing_handler(socket) == EXIT_SUCCESS);
			player_buffer_len = 0;
		}

		if (player_buffer_len == 0) {
			set_player_buffer_replace(socket, cmd);
		} else {
			set_player_buffer_append(socket, cmd);
		}

		if (++commands_on_line < 5) {
			set_player_buffer_append(socket, "\t");
			if (strlen((char *)cmd) < 7)
				set_player_buffer_append(socket, "\t");
		}

		if (commands_on_line == 5) {
			commands_on_line = 0;
			assert(outgoing_handler(socket) == EXIT_SUCCESS);
		}
	}

	if (strlen((char *)get_player_buffer(socket)) != 0)
		set_player_buffer_append(socket, "\n");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return true;
}

int32_t greet_player(const int32_t socket)
{
	set_player_buffer_replace(socket, "WELCOME.\n\n");
	set_player_buffer_append(socket, "Please provide a NAME; this can be two words and up to ");
	set_player_buffer_append(socket, NAMES_MAX_STR);
	set_player_buffer_append(socket, " characters long in total.\n\nIf you've "
			"already created a character, enter your previous name to resume.\n\n");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

int32_t print_player_speech(const int32_t socket)
{
	struct player_live_record *player = get_player_by_socket(socket);

	#define SKIP_SAY_TOKEN 4 // length req'd for the actual say command + the space after that
	uint8_t *buffer = calloc(BUFFER_LENGTH+1, sizeof(uint8_t));
	void *loc_in_buf = NULL;

	loc_in_buf = mempcpy(buffer, player->name, strlen((char *)player->name));
	loc_in_buf = mempcpy(loc_in_buf, " says: ", 7);

	int32_t to_add = strlen((char *)&player->buffer[SKIP_SAY_TOKEN]);
	if (to_add + strlen((char *)buffer) > BUFFER_LENGTH)
		to_add = BUFFER_LENGTH - strlen((char *)buffer);

	loc_in_buf = mempcpy(loc_in_buf, &player->buffer[SKIP_SAY_TOKEN], to_add);

	print_to_other_players(socket, buffer);

#ifdef DEBUG
	printf("print_player_speech to others: %s\n", buffer);
#endif

	free(buffer);

	uint8_t substr[to_add];
	memset(substr, '\0', to_add);

	memcpy(substr, "You say: ", 9);
	memcpy(&substr[9], &player->buffer[SKIP_SAY_TOKEN], to_add);
	memcpy(&substr[9+to_add], "\n", 1);

	set_player_buffer_replace(socket, substr);

#ifdef DEBUG
	printf("print_player_speech to them: %s (%lu)\n", player->buffer, strlen((char *)player->buffer));
#endif

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

static int32_t print_to_other_players(const int32_t socket, const uint8_t *buffer)
{
	struct coordinates coords = get_player_coords(socket);

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct player_live_record *player = get_player(i);

		if (player->socket_num == socket)
			continue;

		if (is_in_same_room(player->socket_num, coords) == true) {
			set_player_buffer_replace(player->socket_num, buffer);
			set_player_buffer_append(player->socket_num, "\n");
			assert(outgoing_handler(player->socket_num) == EXIT_SUCCESS);
		}
	}

	return EXIT_SUCCESS;
}
