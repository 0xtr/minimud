#include "handling_incoming_players.h"

static int32_t check_player_pass(const struct PlayerDBRecord *player, const uint8_t *pw);

int32_t check_for_highest_socket_num(void)
{
	struct PlayerLiveRecord *tmp = get_player_head();
	int32_t fdmax = 0;
	if (tmp == NULL)
		return fdmax;

	while ((tmp = tmp->next) != NULL) {
		if (tmp->socket_num > fdmax)
			fdmax = tmp->socket_num;
	}

	return fdmax;
}

static int32_t check_player_pass(const struct PlayerDBRecord *player, const uint8_t *pw)
{
	// TODO: do this and pass to insert_player
	const size_t PASSWORD_LEN = (strlen((char *)pw) > BUFFER_LENGTH) ? BUFFER_LENGTH : strlen((char *)pw);

	uint8_t *salt_and_pw = calloc(BUFFER_LENGTH + SALT_LENGTH, sizeof(uint8_t));
	void *append_pw = mempcpy(salt_and_pw, player->salt, strlen((char *)player->salt));
	append_pw = mempcpy(append_pw, pw, PASSWORD_LEN);
	append_pw = mempcpy(append_pw, "\0", 1);
	
	uint8_t *hash_result = calloc(HASH_LENGTH, sizeof(uint8_t));
	const int32_t rv = bcrypt_checkpass((char *)salt_and_pw, (char *)player->hash);

	memset(hash_result, '\0', HASH_LENGTH);
	free(hash_result);
	memset(salt_and_pw, '\0', BUFFER_LENGTH + SALT_LENGTH);
	free(salt_and_pw);

	if (rv == -1)
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}

int32_t handle_existing_pass(const int32_t socket, const uint8_t *command)
{
	struct PlayerDBRecord *player = lookup_player(get_player_name(socket));
	if (player == NULL) {
		print_to_player(socket, UNABLE_TO_RETRIEVE_CHAR);
		set_player_wait_state(socket, THEIR_NAME);
		return EXIT_FAILURE;
	}

	if (check_player_pass(player, command) == EXIT_FAILURE) {
		free(player);
		print_to_player(socket, INCORRECT_PASSWORD);
		set_player_wait_state(socket, THEIR_NAME);
		return EXIT_FAILURE;
	}

	struct Coordinates coords = get_player_coords(socket);
	if (lookup_room(coords) == NULL) {
		coords.x = coords.y = coords.z = -1;
		assert(adjust_player_location(socket, coords) == EXIT_SUCCESS);
		fprintf(stdout, "[INFO] Moving player %d from a nonexistent room.\n", socket);
	}

	free(player);

	print_to_player(socket, SHOWROOM);
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);

	return EXIT_SUCCESS;
}

int32_t handle_new_pass(const int32_t socket, const uint8_t *command)
{
	if (strcmp((char *)command, (char *)get_player_store(socket)) != 0 || 
	   (strlen((char *)command) != strlen((char *)get_player_store(socket)))) {
		print_to_player(socket, MISMATCH_PW_SET);
		set_player_wait_state(socket, THEIR_NAME);
		clear_player_store(socket);
		return EXIT_FAILURE;
	}

	print_to_player(socket, ATTEMPT_CREATE_USR);

	if (insert_player(get_player_name(socket), command, socket) == -1) {
		print_to_player(socket, PLAYER_CREATION_FAILED);
		shutdown_socket(socket);
		return EXIT_FAILURE;
	}

	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	print_to_player(socket, SHOWROOM);

	fprintf(stdout, "Player name %s connected on socket %d.\n", get_player_name(socket), socket);

	return EXIT_SUCCESS;
}

int32_t set_player_confirm_new_pw(const int32_t socket, const uint8_t *command)
{
	init_player_store(socket);
	set_player_store_replace(socket, command);

	print_to_player(socket, REQUEST_PW_CONFIRM);
	set_player_wait_state(socket, THEIR_PASSWORD_NEWFINAL);

	return EXIT_SUCCESS;
}

#define NUM_RESERVED_WORDS 5
static const uint8_t RESERVED_WORDS[NUM_RESERVED_WORDS][15] = { "admin", "root", "ADMIN", "Admin", "Administrator" };

_Bool check_if_name_is_reserved(const int32_t socket, const uint8_t *name)
{
	for (size_t i = 0; i < NUM_RESERVED_WORDS; ++i) {
		if (strstr((char *)name, (char *)RESERVED_WORDS[i]) != NULL) {
			print_to_player(socket, NAME_UNAVAILABLE);
			print_to_player(socket, NAME_NOT_WITHIN_PARAMS);
			return true;
		}
	}
	for (size_t i = 0; i < get_num_of_available_cmds(); ++i) {
		/*
		if (memcmp(get_command(i), name, strlen((char *)name)) == 0) {
			print_to_player(socket, NAME_UNAVAILABLE);
			print_to_player(socket, NAME_NOT_WITHIN_PARAMS);
			return true;
		}
		*/
	}
	return false;
}

#define is_not_a_space get_player_buffer(socket)[i] != ' '
_Bool check_if_name_is_valid(const int32_t socket, const uint8_t *name)
{
	if (strlen((char *)name) > NAMES_MAX || strlen((char *)name) < NAMES_MIN) {
		print_to_player(socket, NAME_NOT_WITHIN_PARAMS);
		return false;
	}

	for (size_t i = 0; i < NAMES_MAX; ++i) {
		if (get_player_buffer(socket)[i] == 0)
			break;

		if (!isalpha(get_player_buffer(socket)[i]) && is_not_a_space)
			return false;
	}

	return true;
}

_Bool check_if_player_is_already_online(const size_t socket)
{
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (strcmp((char *)get_player_name(socket), (char *)get_player_name(i)) == 0 && i != socket) {
			print_to_player(socket, PLAYER_ALREADY_ONLINE);
			set_player_wait_state(socket, THEIR_NAME);
			return true;
		}
	}

	return false;
}

