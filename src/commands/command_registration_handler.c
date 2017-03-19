#include "command_registration_handler.h"

static size_t MAX_COMMAND_LEN = 0;
static size_t NUM_OF_AVAILABLE_COMMANDS = 0;
static size_t TOTAL_LENGTH_OF_ALL_CMDS = 0;
static _Bool  PARSED_ALREADY = false;
struct Command *head = NULL;
struct Command *tail = NULL;

static void set_line_len_if_greater (const uint8_t *line);
static _Bool line_begins_with_valid_chars(const uint8_t *line);
static _Bool line_len_is_greater(const uint8_t *line);

size_t get_max_command_len(void)
{
	return MAX_COMMAND_LEN;
}

int32_t parse_clist(void)
{
	if (PARSED_ALREADY == true)
		return EXIT_SUCCESS;

	char *line = NULL;
	ssize_t ret = 0;
	size_t line_len = 0;
	FILE *fp = NULL;
	struct Command *curr;

	assert(access("src/commands/COMMAND_LIST.txt", F_OK) == 0);
	assert((fp = fopen("src/commands/COMMAND_LIST.txt", "r")) != NULL);

	for (size_t i = 0; ; ++i) {
		if ((ret = getline(&line, &line_len, fp)) == -1) {
			break;
		}

		if (line_begins_with_valid_chars((uint8_t*)line) == false)
			continue;

		const int32_t len = strlen(line);

		curr = (struct Command *)calloc(sizeof(struct Command), sizeof(struct Command));
		curr->cname = calloc(len, sizeof(uint8_t));

		if (head == NULL)
			head = curr;

		if (tail != NULL) {
			tail->next = curr;
			curr->prev = tail;
		}
		tail = curr;

		if (line[len] == '.')
			line[len] = '\0';
		if (isspace(line[len - 1]))
			line[len - 1] = '\0';

		memcpy(curr->cname, line, len);

		++NUM_OF_AVAILABLE_COMMANDS;
		TOTAL_LENGTH_OF_ALL_CMDS += strlen((char *)line);

		set_line_len_if_greater((uint8_t*)line);
	}
	free(line);

	PARSED_ALREADY = true;

	return EXIT_SUCCESS;
}

static void set_line_len_if_greater (const uint8_t *line)
{
	if (line_len_is_greater(line) == true)
		MAX_COMMAND_LEN = strlen((char *)line);
}

static _Bool line_len_is_greater(const uint8_t *line)
{
	if (strlen((char *)line) > MAX_COMMAND_LEN)
		return true;
	return false;
}

size_t get_num_of_available_cmds(void)
{
	return NUM_OF_AVAILABLE_COMMANDS;
}

static _Bool line_begins_with_valid_chars(const uint8_t *line)
{
	return line[0] != '#' && strlen((char *)line) != 0 &&
	       line[0] != '.' && line[0] != '\n';
}

size_t get_total_length_of_all_cmds(void)
{
	return TOTAL_LENGTH_OF_ALL_CMDS;
}

int32_t check_clist(const int32_t socket, const uint8_t *command)
{
	struct Command *tmp = head;
	const size_t cmd_len = strlen((char *)command);

	while (tmp != NULL) {
		if (memcmp(command, tmp->cname, cmd_len) != 0) {
			tmp = tmp->next;
			continue;
		}

		if (is_direction(command))
			assert(move_player(socket, get_direction_as_number(command)));

		// isroomcmd
		// isgamecmd
		if (memcmp(command, "look", cmd_len) == 0 || command[0] == 'l') {
			print_to_player(socket, SHOWROOM);
		} else if (memcmp(command, "say", cmd_len) == 0) {
			print_player_speech_to_player(socket, command);
			print_not_player(socket, command, ROOM_ONLY);
		} else if (memcmp(command, "quit", cmd_len) == 0) {
			shutdown_socket(socket);
		} else if (memcmp(command, "commands", cmd_len) == 0) {
			print_to_player(socket, SHOWCMDS);
		} else if (memcmp(command, "players", cmd_len) == 0) {
			print_to_player(socket, LISTPLAYERS);

		////////// ROOMS //////////
		// if (issetcmd(command[0]) == 0) {
		//     set_room(socket_num, command);
		// }
		} else if (memcmp(command, "setrname", cmd_len) == 0) {
			print_to_player(socket, PRINT_PROVIDE_NEW_ROOM_NAME);
			set_player_wait_state(socket, WAIT_ENTER_NEW_ROOM_NAME);
			set_player_holding_for_input(socket, 1);
		} else if (memcmp(command, "setrdesc", cmd_len) == 0) {
			print_to_player(socket, PRINT_PROVIDE_NEW_ROOM_DESC);
			set_player_wait_state(socket, WAIT_ENTER_NEW_ROOM_DESC);
			set_player_holding_for_input(socket, 1);
		} else if (memcmp(command, "setrexit", cmd_len) == 0) {
			print_to_player(socket, PRINT_PROVIDE_ROOM_EXIT_NAME);
			set_player_wait_state(socket, WAIT_ENTER_EXIT_NAME);
			set_player_holding_for_input(socket, 1);
		} else if (memcmp(command, "setrflag", cmd_len) == 0) {
			print_to_player(socket, PRINT_PROVIDE_ROOM_FLAG_NAME);
			set_player_wait_state(socket, WAIT_ENTER_FLAG_NAME);
			set_player_holding_for_input(socket, 1);
			// if (isroomexistcmd(command[0]) == 0) {
			//     something
			// }
		} else if (memcmp(command, "mkroom", cmd_len) == 0) {
			print_to_player(socket, PRINT_ROOM_CREATION_GIVE_DIR);
			set_player_wait_state(socket, WAIT_ROOM_CREATION_DIR);
			set_player_holding_for_input(socket, 1);
		} else if (memcmp(command, "rmroom", cmd_len) == 0) {
			print_to_player(socket, PRINT_ROOM_REMOVAL_CHECK);
			set_player_wait_state(socket, WAIT_ROOM_REMOVAL_CHECK);
			set_player_holding_for_input(socket, 1);
		}

		/* we only reach this if we found a match */
		return EXIT_SUCCESS; 
	}

	return EXIT_FAILURE;
}

size_t set_num_of_available_cmds(void)
{
	struct Command *tmp = head;
	int32_t commands = 0;

	while (tmp)
		++commands;

	return commands;
}

uint8_t *get_command(const int32_t cmd)
{
	struct Command *tmp = head;

	for (size_t i = 0; i < (size_t)cmd; ++i)
		tmp = tmp->next;

	return tmp->cname;
}
