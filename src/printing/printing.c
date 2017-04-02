#include "printing.h"

static int32_t print_to_other_players(const int32_t socket, const uint8_t *buffer);
static _Bool set_buffer_for_movement(const int32_t socket, const int32_t argument);
static _Bool print_all_commands(const int32_t socket);
static _Bool build_room_image(const int32_t socket);
static _Bool is_in_same_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket);
static void append_coordinates_for_printing(const int32_t socket, const struct Coordinates coords);

int32_t print_to_player(const int32_t socket, const int32_t argument)
{
	#define IS_DIRECTION_ARG (argument >= NORTH_DIR && argument <= RETURN_ORIGIN_DIR)
	_Bool prints_in_func = false;

	switch (argument) {
	case PROMPT: 
		set_player_buffer_replace(socket, "");
		break;
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
		set_player_buffer_append(socket, "].\n");
		set_player_buffer_append(socket, "Please provide a password less than ");
		set_player_buffer_append(socket, BUFFER_LENGTH_STR);
		set_player_buffer_append(socket, " characters long.");
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
		set_player_buffer_replace(socket, "You're trying to delete this room. Are you sure you want to do this?\nType only y/Y to confirm.\n");
		break;
	case PRINT_ROOM_REMOVAL_CONFIRM:
		set_player_buffer_replace(socket, "Again, confirm room deletion with y/Y - deleting: ");
		set_player_buffer_append(socket, get_player_store(socket));
		set_player_buffer_append(socket, ".\n");
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
	case PRINT_ROOM_CREATION_FAILURE:
		set_player_buffer_replace(socket, "A room already exists in that direction. Exiting editor.\n");
		break;
	case PRINT_REMOVED_FROM_ROOM:
		set_player_buffer_replace(socket, "You've been moved from your current room by the system; the owning player may have deleted it.\n");
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
	if (prints_in_func == false)
		assert(outgoing_handler(socket) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

static _Bool set_buffer_for_movement (const int32_t socket, const int32_t argument) {
	switch (argument) {
	case 0:
		set_player_buffer_replace(socket, "Returning to the central room.\n");
		return EXIT_SUCCESS;
	case 1:
		set_player_buffer_replace(socket, "Moving NORTH.\n");
		return EXIT_SUCCESS;
	case 2:
		set_player_buffer_replace(socket, "Moving EAST.\n");
		return EXIT_SUCCESS;
	case 3:
		set_player_buffer_replace(socket, "Moving SOUTH.\n");
		return EXIT_SUCCESS;
	case 4:
		set_player_buffer_replace(socket, "Moving WEST.\n");
		return EXIT_SUCCESS;
	case 5:
		set_player_buffer_replace(socket, "Moving DOWN.\n");
		return EXIT_SUCCESS;
	case 6:
		set_player_buffer_replace(socket, "Moving UP.\n");
		return EXIT_SUCCESS;
	case 7:
		set_player_buffer_replace(socket, "Moving NORTHWEST.\n");
		return EXIT_SUCCESS;
	case 8:
		set_player_buffer_replace(socket, "Moving NORTHEAST.\n");
		return EXIT_SUCCESS;
	case 9:
		set_player_buffer_replace(socket, "Moving SOUTHWEST.\n");
		return EXIT_SUCCESS;
	case 10:
		set_player_buffer_replace(socket, "Moving SOUTHEAST.\n");
		return EXIT_SUCCESS;
	default:
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}

static _Bool is_in_same_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket)
{
	struct Coordinates coords = get_player_coords(socket);
	return coords.x == x && coords.y == y && coords.z == z;
}

static _Bool build_room_image (const int32_t socket) {
	struct Coordinates coords = get_player_coords(socket);
	struct RoomRecord *map = lookup_room(coords);

	set_player_buffer_replace(socket, (map->rname == NULL) ? (void *)"NULL SPACE" : (void *)map->rname);
	append_coordinates_for_printing(socket, coords);
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	set_player_buffer_replace(socket, "Exits:");

	if (map->north == true)
		set_player_buffer_append(socket, " NORTH");
	if (map->south == true)
		set_player_buffer_append(socket, " SOUTH");
	if (map->east == true)
		set_player_buffer_append(socket, " EAST");
	if (map->west == true)
		set_player_buffer_append(socket, " WEST");
	if (map->up == true)
		set_player_buffer_append(socket, " U");
	if (map->down == true)
		set_player_buffer_append(socket, " D");
	if (map->northeast == true)
		set_player_buffer_append(socket, " NE");
	if (map->southeast == true)
		set_player_buffer_append(socket, " SE");
	if (map->southwest == true)
		set_player_buffer_append(socket, " SW");
	if (map->northwest == true)
		set_player_buffer_append(socket, " NW");
	if (strlen((char *)get_player_buffer(socket)) == 6)
		set_player_buffer_append(socket, " NONE");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	set_player_buffer_replace(socket, 
			(map ->rdesc == NULL) ?
			(void *)"It is pitch black. You are "
			"likely to be eaten by a null character."
			: (void *)map->rdesc);
	set_player_buffer_append(socket, "\n");
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	// now show the players in room here...
	// doesn't scale, TODO
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (get_player_name(i) != get_player_name(socket)) {
			if (is_in_same_room(coords.x, coords.y, coords.z, i)) {
				set_player_buffer_replace(i, get_player_name(i));
				set_player_buffer_append(i, (uint8_t *)" is here too.\n");
				assert(outgoing_handler(socket) == EXIT_SUCCESS);
			}
		}
	}

	free(map);

	return true;
}

static void append_coordinates_for_printing(const int32_t socket, const struct Coordinates coords)
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

	set_player_buffer_replace(socket, "Available commands:\n> ");

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
	set_player_buffer_append(socket, " characters long in total.\n\nIf you've already created a character, enter your previous name to resume.");

	assert(outgoing_handler(socket) == EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

int32_t print_player_speech(const int32_t socket)
{
	#define TOKEN_SAY_CMD_LEN 4 // length req'd for the actual say command + the space after that
	#define TOKEN_YOU_SAY_LEN 9 // length req'd for player to see You say: 
	uint8_t *buffer = calloc(BUFFER_LENGTH, sizeof(uint8_t));
	void *loc_in_buf = NULL;

	loc_in_buf = mempcpy(buffer, get_player_name(socket), strlen((char *)get_player_name(socket)));
	loc_in_buf = mempcpy(loc_in_buf, " says: ", BUFFER_LENGTH - strlen((char *)buffer));
	loc_in_buf = mempcpy(loc_in_buf, &get_player_buffer(socket)[TOKEN_SAY_CMD_LEN], BUFFER_LENGTH - strlen((char *)buffer));

	print_to_other_players(socket, buffer);
#ifdef DEBUG
	printf("print_player_speech: %s\n", buffer);
#endif

	free(buffer);

	set_player_buffer_replace(socket, "You say: ");
	set_player_buffer_append(socket, &get_player_buffer(socket)[TOKEN_SAY_CMD_LEN]);

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

#define is_in_same_room \
	this_player.x == coords.x && this_player.y == coords.y && this_player.z == coords.z

static int32_t print_to_other_players(const int32_t socket, const uint8_t *buffer)
{
	struct Coordinates coords = get_player_coords(socket);

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		struct Coordinates this_player = get_player_coords(i);

		if (is_in_same_room) {
			set_player_buffer_replace(i, buffer);
			assert(outgoing_handler(i) == EXIT_SUCCESS);
		}
	}

	return EXIT_SUCCESS;
}
