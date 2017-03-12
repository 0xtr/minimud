#include "handling_incoming_players.h"

int32_t check_for_highest_socket_num(void)
{
	struct Player *tmp = get_player_head();
	int32_t fdmax = 0;
	if (tmp == NULL)
		return fdmax;

	while ((tmp = tmp->next) != NULL) {
		if (tmp->socket_num > fdmax)
			fdmax = tmp->socket_num;
	}

	return fdmax;
}

int32_t handle_existing_pass(const int32_t socket, const uint8_t *command)
{
	if (get_existing_player_hash(socket) == -1) {
		print_to_player(socket, UNABLE_TO_RETRIEVE_CHAR);
		set_player_wait_state(socket, THEIR_NAME);
		return EXIT_FAILURE;
	}

	if (bcrypt_checkpass((char *)command, (char *)get_existing_player_hash(socket)) == -1) {
		print_to_player(socket, INCORRECT_PASSWORD);
		set_player_wait_state(socket, THEIR_NAME);
		return EXIT_FAILURE;
	}

	if (lookup_room(get_player_coord(X_COORD_REQUEST, socket),
			get_player_coord(Y_COORD_REQUEST, socket),
			get_player_coord(Z_COORD_REQUEST, socket), -1) == 0) {
		assert(adjust_player_location(socket, -1, -1, -1) == EXIT_SUCCESS);
		fprintf(stdout, "[INFO] Moving player %d from a nonexistent room.\n", socket);
	}

	print_to_player(socket, SHOWROOM);
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);

	return EXIT_SUCCESS;
}

int32_t handle_new_pass(const int32_t socket, const uint8_t *command)
{
	if (strcmp((char *)command, (char *)get_player_store(socket)) != 0 && 
	   (strlen((char *)command) != strlen((char *)get_player_store(socket)))) {
		print_to_player(socket, MISMATCH_PW_SET);
		set_player_wait_state(socket, THEIR_NAME);
		clear_player_store(socket);
		return EXIT_FAILURE;
	}

	print_to_player(socket, ATTEMPT_CREATE_USR);
	if ((insert_player(get_player_name(socket), command, socket)) == -1) {
		print_to_player(socket, PLAYER_CREATION_FAILED);
		shutdown_socket(socket);
		return EXIT_FAILURE;
	}

	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	print_to_player(socket, SHOWROOM);
	fprintf(stdout, "Player (num %d), name %s has connected.\n", socket, get_player_name(socket));

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
			return true;
		}
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

		if (!isalpha(get_player_buffer(socket)[i]) && is_not_a_space) {
			return false;
		}
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

