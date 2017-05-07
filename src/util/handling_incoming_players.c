#include "handling_incoming_players.h"

#define NUM_RESERVED_WORDS 5
static const uint8_t RESERVED_WORDS[NUM_RESERVED_WORDS][15] = { "admin", "root", "administrator", "system" };

static int32_t check_player_pass(struct player_db_record *player, const uint8_t *pw);

int32_t check_for_highest_socket_num(void)
{
	struct player_live_record *tmp = get_player_head();
	int32_t fdmax = 0;
	if (tmp == NULL)
		return fdmax;

	while ((tmp = tmp->next) != NULL) {
		if (tmp->socket_num > fdmax)
			fdmax = tmp->socket_num;
	}

	return fdmax;
}

static int32_t check_player_pass(struct player_db_record *player, const uint8_t *pw)
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

int32_t handle_existing_pass(struct player_live_record *player, const uint8_t *command)
{
	struct player_db_record *player_db = lookup_player(player->name);

	if (player_db == NULL) {
		print_to_player(player, UNABLE_TO_RETRIEVE_CHAR);

		player->wait_state = THEIR_NAME;

		return EXIT_FAILURE;
	}

	const int32_t pid = player_db->id;
	// clear pnames if this fails, cause failures are being logged
	int32_t rv = check_player_pass(player_db, command);
	free(player_db);

	if (rv == EXIT_FAILURE) {
		print_to_player(player, INCORRECT_PASSWORD);

		memset(player->name, '\0', strlen((char *)player->name));
		player->wait_state = THEIR_NAME;

		return EXIT_FAILURE;
	}

	struct coordinates coords = get_player_coords(player);
	struct room_atom *room = lookup_room(coords);

	if (room == NULL)
		assert(adjust_player_location(player, 0) == EXIT_SUCCESS);

	store_player_id(player, pid);

	print_room_to_player(player, room);

	reset_player_state(player);

	player->connected = true;

	fprintf(stdout, "Player name %s connected on socket %d.\n", player->name, 
			player->socket_num);
	
	free(room);

	return EXIT_SUCCESS;
}

int32_t handle_new_pass(struct player_live_record *player, const uint8_t *command)
{
	if (strcmp((char *)command, (char *)player->store) != 0 || 
	   (strlen((char *)command) != strlen((char *)player->store))) {
		print_to_player(player, MISMATCH_PW_SET);

		player->wait_state = THEIR_NAME;
		clear_player_store(player);
		return EXIT_FAILURE;
	}

	print_to_player(player, ATTEMPT_CREATE_USR);

	if (insert_player(player, command) == -1) {
		print_to_player(player, PLAYER_CREATION_FAILED);
		shutdown_socket(player);
		return EXIT_FAILURE;
	}

	reset_player_state(player);

	struct room_atom *room = lookup_room_by_id(0);

	print_room_to_player(player, room);

	free(room);

	fprintf(stdout, "Player name %s connected on socket %d.\n", player->name, 
			player->socket_num);

	return EXIT_SUCCESS;
}

int32_t set_player_confirm_new_pw(struct player_live_record *player, const uint8_t *command)
{
	init_player_store(player);

	set_player_store_replace(player, command);

	print_to_player(player, REQUEST_PW_CONFIRM);

	player->wait_state = THEIR_PASSWORD_NEWFINAL;

	return EXIT_SUCCESS;
}

_Bool check_if_name_is_reserved(struct player_live_record *player, const uint8_t *name)
{
	for (size_t i = 0; i < NUM_RESERVED_WORDS; ++i) {
		if (strcasecmp((char *)name, (char *)RESERVED_WORDS[i]) == 0) {
			print_to_player(player, NAME_UNAVAILABLE);
			print_to_player(player, NAME_NOT_WITHIN_PARAMS);
			return true;
		}
	}

	for (size_t i = 0; i < get_num_of_available_cmds(); ++i) {
		/*
		if (memcmp(get_command(i), name, strlen((char *)name)) == 0) {
			print_to_player(player, NAME_UNAVAILABLE);
			print_to_player(player, NAME_NOT_WITHIN_PARAMS);
			return true;
		}
		*/
	}

	return false;
}

_Bool check_if_name_is_valid(struct player_live_record *player, const uint8_t *name)
{
	if (strlen((char *)name) > NAMES_MAX || strlen((char *)name) < NAMES_MIN) {
		print_to_player(player, NAME_NOT_WITHIN_PARAMS);
		return false;
	}

	for (size_t i = 0; i < NAMES_MAX; ++i) {
		int32_t c = player->buffer[i];
		if (c == 0)
			break;

		if (!isalpha(c) && c != ' ')
			return false;
	}

	return true;
}

_Bool check_if_player_is_already_online(struct player_live_record *player, const uint8_t *name)
{
	printf("num of players on %lu\n", get_num_of_players());

	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (memcmp(name, get_player_by_index(i)->name, strlen((char *)name)) != 0)
			continue;

		print_to_player(player, PLAYER_ALREADY_ONLINE);

		player->wait_state = THEIR_NAME;

		return true;
	}

	return false;
}

