#include "print_to_player.h"

static _Bool set_buffer_for_movement(const int32_t socket, const int32_t argument);
static _Bool print_all_commands(const int32_t socket);
static _Bool build_room_image(const int32_t socket);
static _Bool is_in_same_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket);

int32_t print_to_player(const int32_t socket, const int32_t argument)
{
	_Bool already_sent = 0;
	#define IS_DIRECTION_ARG (argument >= NORTH_DIR && argument <= RETURN_ORIGIN_DIR)

	switch (argument) {
		case PROMPT: 
			set_player_buffer_replace(socket, (uint8_t *)"> ");
			break;
		case INVALCMD:
			set_player_buffer_replace(socket, (uint8_t *)"Invalid command. Type \'commands\'.\n");
			break;
		case SHOWCMDS:
			print_all_commands(socket);
			already_sent = true;
			break;
		case SHOWROOM:
			build_room_image(socket);
			already_sent = true;
			break;
		case INVALDIR:
			set_player_buffer_replace(socket, (uint8_t *)"Cannot move in that direction. Type 'look' to view room.");
			break;
		case REQUEST_PW_FOR_NEW:
			set_player_buffer_replace(socket, (uint8_t *)"You've provided the name [");
			set_player_buffer_append(socket, get_player_name(socket));
			set_player_buffer_append(socket, (uint8_t *)"].\n");
			set_player_buffer_append(socket, (uint8_t *)"Please provide a password less than ");
			set_player_buffer_append(socket, (uint8_t *)BUFFER_LENGTH);
			set_player_buffer_append(socket, (uint8_t *)" characters long.\n");
			break;
		case REQUEST_PW_CONFIRM:
			set_player_buffer_replace(socket, (uint8_t *)"Please confirm your password by typing it out once more.\n");
			break;
		case REQUEST_PW_FOR_EXISTING:
			set_player_buffer_replace(socket, (uint8_t *)"Please provide your password.");
			break;
		case ATTEMPT_CREATE_USR:
			set_player_buffer_replace(socket, (uint8_t *)"Attempting to create your character...\n");
			break;
		case MISMATCH_PW_SET:
			set_player_buffer_replace(socket, (uint8_t *)"Password mismatch. Start over.\nPlease provide a NAME.\n");
			break;
		case PLAYER_CREATION_FAILED:
			set_player_buffer_replace(socket, (uint8_t *)"Character creation failed. You should never see this.\n");
			break;
		case PLAYER_ALREADY_ONLINE: 
			set_player_buffer_replace(socket, (uint8_t *)"That player is already connected. Please provide another name.\n");
			break;
		case INCORRECT_PASSWORD:
			set_player_buffer_replace(socket, (uint8_t *)"Incorrect password. Restarting.\nPlease provide a NAME.\n");
			break;
		case UNABLE_TO_RETRIEVE_CHAR:
			set_player_buffer_replace(socket, (uint8_t *)"Couldn't retrieve your character.\n");
			break;
		case NAME_UNAVAILABLE:
			set_player_buffer_replace(socket, (uint8_t *)"That name is unavailable.\n");
			break;
		case ALPHANUM_NAMES_ONLY:
			set_player_buffer_replace(socket, (uint8_t *)"Only alphanumeric characters are permitted.\n");
			break;
		case NAME_TOO_LONG:
			set_player_buffer_replace(socket, (uint8_t *)"Name too long. Maximum length is ");
			set_player_buffer_append(socket, (uint8_t *)NAMES_MAX);
			set_player_buffer_append(socket, (uint8_t *)" characters. Try again.\nPlease provide a NAME.\n");
			break;
		case NAME_TOO_SHORT:
			set_player_buffer_replace(socket, (uint8_t *)"Name too short. Minimum length is ");
			set_player_buffer_append(socket, (uint8_t *)NAMES_MIN);
			set_player_buffer_append(socket, (uint8_t *)" characters. Try again.\nPlease provide a NAME.\n");
			break;
		case PRINT_PROVIDE_NEW_ROOM_NAME:
			set_player_buffer_replace(socket, (uint8_t *)"Enter a new room name, of up to ");
			set_player_buffer_append(socket, (uint8_t *)MAX_ROOM_NAME);
			set_player_buffer_append(socket, (uint8_t *)" chars.\n");
			break;
		case PRINT_PROVIDE_NEW_ROOM_DESC:
			set_player_buffer_replace(socket, (uint8_t *)"Enter a new room description, of up to ");
			set_player_buffer_append(socket, (uint8_t *)BUFFER_LENGTH);
			set_player_buffer_append(socket, (uint8_t *)" chars.\n");
			break;
		case PRINT_CONFIRM_NEW_ROOM_DESC:
			set_player_buffer_replace(socket, (uint8_t *)"Confirm the new description by typing Y/y. You entered:");
			set_player_buffer_append(socket, get_player_store(socket));
			set_player_buffer_replace(socket, (uint8_t *)"\nIf this is wrong, type something other than Y/y.");
			break;
		case PRINT_CONFIRM_NEW_ROOM_NAME:
			set_player_buffer_replace(socket, (uint8_t *)"Confirm the new name by typing Y/y. You entered:");
			set_player_buffer_append(socket, get_player_store(socket));
			set_player_buffer_replace(socket, (uint8_t *)"\nIf this is wrong, type something other than Y/y.");
			break;
		case PRINT_ADJUSTMENT_SUCCESSFUL:
			set_player_buffer_replace(socket, (uint8_t *)"Room adjusted successfully. Exiting editor.\n");
			break;
		case PRINT_COULDNT_ADJUST_ROOM:
			set_player_buffer_replace(socket, (uint8_t *)"Some kind of error occurred. Room adjustment failed. Exiting editor.\n");
			break;
		case PRINT_EXITING_CMD_WAIT:
			set_player_buffer_replace(socket, (uint8_t *)"Exiting editor - returning you to the (real) world.\n");
			break;
		case PRINT_INSUFFICIENT_PERMISSIONS:
			set_player_buffer_replace(socket, (uint8_t *)"You don't have permission to do that.\n");
			break;
		case PRINT_ROOM_EXIT_CHANGED:
			set_player_buffer_replace(socket, (uint8_t *)"Room exit changed.\n");
			break;
		case PRINT_ROOM_FLAG_CHANGED:
			set_player_buffer_replace(socket, (uint8_t *)"Room flag toggled.\n");
			break;
		case PRINT_ROOM_REMOVAL_CHECK:
			set_player_buffer_replace(socket, (uint8_t *)"You're trying to delete this room. Are you sure you want to do this?\nType only y/Y to confirm.");
			break;
		case PRINT_ROOM_REMOVAL_CONFIRM:
			set_player_buffer_replace(socket, (uint8_t *)"Again, confirm room deletion with y/Y - deleting: ");
			set_player_buffer_append(socket, get_player_store(socket));
			set_player_buffer_append(socket, (uint8_t *)".\n");
			break;
		case PRINT_ROOM_REMOVAL_SUCCESS:
			set_player_buffer_replace(socket, (uint8_t *)"Room removed successfully.\n");
			break;
		case PRINT_ROOM_REMOVAL_FAILURE:
			set_player_buffer_replace(socket, (uint8_t *)"Room removal failed - you are not the owner of this room.\n");
			break;
		case PRINT_ROOM_CREATION_GIVE_DIR:
			set_player_buffer_replace(socket, (uint8_t *)"Which direction are you trying to create a room in? (from current)\n");
			break;
		case PRINT_ROOM_CREATION_CONFIRMALL:
			set_player_buffer_replace(socket, (uint8_t *)"You're adding a room in the direction of: ");
			set_player_buffer_append(socket, get_player_store(socket));
			set_player_buffer_append(socket, (uint8_t *)". Confirm this by typing y/Y, or decline by typing anything else.\n");
			break;
		case PRINT_ROOM_CREATION_CANNOT:
			set_player_buffer_replace(socket, (uint8_t *)"Room creation couldn't be completed right now.\n");
			break;
		case PRINT_ROOM_CREATION_SUCCESS:
			set_player_buffer_replace(socket, (uint8_t *)"Room creation complete.\n");
			break;
		case PRINT_ROOM_CREATION_FAILURE:
			set_player_buffer_replace(socket, (uint8_t *)"A room already exists in that direction. Exiting editor.\n");
			break;
		case PRINT_REMOVED_FROM_ROOM:
			set_player_buffer_replace(socket, (uint8_t *)"You've been moved from your current room by the system; the owning player may have deleted it.\n");
			break;
		case PRINT_PROVIDE_ROOM_EXIT_NAME:
			set_player_buffer_replace(socket, (uint8_t *)"Which direction are you trying to toggle exit visibility for?\n");
			break;
		case PRINT_PROVIDE_ROOM_FLAG_NAME:
			set_player_buffer_replace(socket, (uint8_t *)"Which flag are you trying to toggle?\n");
			break;
		case PRINT_TOGGLED_ROOM_EXIT:
			set_player_buffer_replace(socket, (uint8_t *)"Room exit visibility toggled. Exiting editor.\n");
			break;
		case PRINT_COULDNT_TOGGLE_EXIT:
			set_player_buffer_replace(socket, (uint8_t *)"Unable to toggle the exit for that room.\n");
			break;
		case PRINT_COULDNT_EXIT_NO_ROOM:
			set_player_buffer_replace(socket, (uint8_t *)"There's no room in that direction.\n");
			break;
		default:
			if (IS_DIRECTION_ARG) {
				set_buffer_for_movement(socket, argument);
			}
	}
	if (already_sent == false) {
		assert(outgoing_handler(socket) == EXIT_SUCCESS);
	}
	return EXIT_SUCCESS;
}

static _Bool set_buffer_for_movement (const int32_t socket, const int32_t argument) {
	switch (argument) {
		case 0:
			set_player_buffer_replace(socket, (uint8_t *)"> Returning to the central room.\n");
			return EXIT_SUCCESS;
		case 1:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving NORTH.\n");
			return EXIT_SUCCESS;
		case 2:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving EAST.\n");
			return EXIT_SUCCESS;
		case 3:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving SOUTH.\n");
			return EXIT_SUCCESS;
		case 4:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving WEST.\n");
			return EXIT_SUCCESS;
		case 5:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving DOWN.\n");
			return EXIT_SUCCESS;
		case 6:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving UP.\n");
			return EXIT_SUCCESS;
		case 7:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving NORTHWEST.\n");
			return EXIT_SUCCESS;
		case 8:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving NORTHEAST.\n");
			return EXIT_SUCCESS;
		case 9:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving SOUTHWEST.\n");
			return EXIT_SUCCESS;
		case 10:
			set_player_buffer_replace(socket, (uint8_t *)"> Moving SOUTHEAST.\n");
			return EXIT_SUCCESS;
		default:
			return EXIT_FAILURE;
	}
	return EXIT_FAILURE;
}

static _Bool is_in_same_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket)
{
	return get_player_coord(X_COORD_REQUEST, socket) == x &&
		get_player_coord(Y_COORD_REQUEST, socket) == y &&
		get_player_coord(Z_COORD_REQUEST, socket) == z;
}

static _Bool build_room_image (const int32_t socket) {
	const int32_t room_x = get_player_coord(X_COORD_REQUEST, socket);
	const int32_t room_y = get_player_coord(Y_COORD_REQUEST, socket);
	const int32_t room_z = get_player_coord(Z_COORD_REQUEST, socket);
	struct Map *map = lookup_room(room_x, room_y, room_z, socket);

	// now show the players in room here...
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (get_player_name(i) != get_player_name(socket)) {
			if (is_in_same_room(room_x, room_y, room_z, i)) {
				set_player_buffer_replace(i, get_player_name(i));
				set_player_store_append(i, (uint8_t *)" is here too.\n");
				assert(outgoing_handler(socket) == EXIT_SUCCESS);
			}
		}
	}

	set_player_buffer_replace(socket, (uint8_t *)"Exits:");
	if (map->north == 1)
		set_player_buffer_append(socket, (uint8_t *)" NORTH");
	if (map->south == 1)
		set_player_buffer_append(socket, (uint8_t *)" SOUTH");
	if (map->east == 1)
		set_player_buffer_append(socket, (uint8_t *)" EAST");
	if (map->west == 1)
		set_player_buffer_append(socket, (uint8_t *)" WEST");
	if (map->up == 1)
		set_player_buffer_append(socket, (uint8_t *)" U");
	if (map->down == 1)
		set_player_buffer_append(socket, (uint8_t *)" D");
	if (map->northeast == 1)
		set_player_buffer_append(socket, (uint8_t *)" NE");
	if (map->southeast == 1)
		set_player_buffer_append(socket, (uint8_t *)" SE");
	if (map->southwest == 1)
		set_player_buffer_append(socket, (uint8_t *)" SW");
	if (map->northwest == 1)
		set_player_buffer_append(socket, (uint8_t *)" NW");
	if (strlen((char *)get_player_buffer(socket)) == 6) {
		set_player_buffer_append(socket, (uint8_t *)" NONE");
	}

	set_player_buffer_append(socket, (uint8_t *)"\n");

	return outgoing_handler(socket);
}

static _Bool print_all_commands(const int32_t socket)
{
	int32_t c = 0;
	int32_t commands_on_line = 0;

	set_player_buffer_replace(socket, (uint8_t *)"> Available commands:\n");
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	if (get_total_length_of_all_cmds() > BUFFER_LENGTH) {
		c = get_total_length_of_all_cmds() % BUFFER_LENGTH;
		(c == 1) ? ++c : c;
	}

	for (size_t i = 0; i < get_num_of_available_cmds(); ++i) {
		if (strlen((char *)get_player_buffer(socket)) + strlen((char *)get_command(i)) > BUFFER_LENGTH) {
			assert(outgoing_handler(socket) == EXIT_SUCCESS);
			continue;
		}
		if (strlen((char *)get_player_buffer(socket)) == 0) {
			set_player_buffer_replace(socket, get_command(i));
		} else {
			set_player_buffer_append(socket, get_command(i));
		}
		if (++commands_on_line != 5)
			set_player_buffer_append(socket, (uint8_t *)"\t");

		if (commands_on_line == 5) {
			commands_on_line = 0;
			assert(outgoing_handler(socket) == EXIT_SUCCESS);
		}
	}

	return EXIT_SUCCESS;
}

int32_t greet_player(const int32_t socket)
{
	set_player_buffer_replace(socket, (uint8_t *)"> WELCOME.\n\n");
	set_player_buffer_append(socket, (uint8_t *)"Please provide a NAME; this can be two words and up to ");
	set_player_buffer_append(socket, (uint8_t *)(char *)NAMES_MAX);
	set_player_buffer_append(socket, (uint8_t *)" characters long in total.\nIf you've already created a character, enter your previous name to resume.");
	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}

int32_t print_player_speech_to_player(const int32_t socket, const uint8_t *say)
{
	#define TOKEN_SAY_CMD_LEN 4 // length req'd for the actual say command + the space after that
	#define TOKEN_YOU_SAY_LEN 9 // length req'd for player to see You say: 
	uint8_t *buffer = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	strncpy((char *)buffer, (char *)get_player_name(socket), strlen((char *)get_player_name(socket)));
	strncat((char *)buffer, (char *)" says: ", BUFFER_LENGTH - strlen((char *)buffer));
	strncat((char *)buffer, (char *)say[TOKEN_SAY_CMD_LEN], BUFFER_LENGTH - strlen((char *)buffer));

	print_not_player(socket, buffer, ROOM_ONLY);
#ifdef DEBUG
	printf("print_player_speech: %s\n", buffer);
#endif

	free(buffer);

	set_player_buffer_replace(socket, (uint8_t *)"You say: ");
	set_player_buffer_append(socket, &say[TOKEN_SAY_CMD_LEN]);

	assert(outgoing_handler(socket) == EXIT_SUCCESS);

	return EXIT_SUCCESS;
}
