#include "player_live_record.h"

static struct player_live_record *head = NULL;
static struct player_live_record *tail = NULL;

struct player_live_record *get_player(const int32_t socket)
{
	struct player_live_record *curr = head;

	if (curr != NULL) {
		while (curr != NULL) {
			if (curr->socket_num == socket)
				break;
			curr = curr->next;
		}
	}

	return curr;
}

struct player_live_record *get_player_by_id(const int32_t id)
{
	struct player_live_record *curr = head;

	if (curr != NULL) {
		while (curr != NULL) {
			if (curr->db_id == id)
				break;
			curr = curr->next;
		}
	}

	return curr;
}

struct player_live_record *get_player_head(void)
{
	return head;
}

struct player_live_record *get_player_by_index(const size_t index)
{
	struct player_live_record *curr = head;

	if (curr == NULL)
		return NULL;

	for (size_t i = 0; i < index; ++i) {
		if (curr == NULL)
			return NULL;

		curr = curr->next;
	}

	return curr;
}

int32_t remove_last_player_record(void)
{
	if (tail == NULL)
		return EXIT_FAILURE;

	struct player_live_record *curr = tail;

	curr = tail->prev;
	free(tail);

	curr->next = NULL;
	tail = curr;

	return EXIT_SUCCESS;
}

int32_t remove_player_by_socket(const int32_t socket)
{
	struct player_live_record *curr = get_player(socket);

	struct player_live_record *prev = curr->prev;
	struct player_live_record *next = curr->next;

	if (tail == curr)
		tail = curr->prev;
	if (head == curr)
		head = curr->next;

	printf("remove by socket %d\n", socket);
	free(curr);

	if (prev != NULL)
		prev->next = next;
	if (next != NULL)
		next->prev = prev;

	return EXIT_SUCCESS;
}

int32_t add_new_player(const int32_t socket)
{
	struct player_live_record *curr = (struct player_live_record *)calloc(
			sizeof(struct player_live_record), sizeof(struct player_live_record));

	if (curr == NULL)
		return EXIT_FAILURE;

	curr->db_id = -1;
    	curr->socket_num = socket;
    	curr->inventory = get_new_player_inventory(socket);
	curr->holding_for_input = true;
	curr->wait_state = THEIR_NAME;
	curr->prev = tail;

	if (tail != NULL)
		tail->next = curr;

	tail = curr;

	if (head == NULL)
		head = curr;

	return EXIT_SUCCESS;
}

int32_t set_player_name(struct player_live_record *player, const uint8_t *name)
{
	memcpy(player->name, name, strlen((char *)name));

	return EXIT_SUCCESS;
}

int32_t set_player_store_replace(struct player_live_record *player, const void *newval)
{
	memset(player->store, '\0', player->store_size);

	int32_t len = strlen((char *)newval);
	if (len > BUFFER_LENGTH)
		len = BUFFER_LENGTH;

	memcpy(player->store, newval, len);
	player->store_size = len;

	return EXIT_SUCCESS;
}

int32_t set_player_store_append(struct player_live_record *player, const void *new)
{
	if (strlen((char *)player->store) + strlen((char *)new) > BUFFER_LENGTH)
		return EXIT_FAILURE;

	memcpy(&player->store[strlen((char *)player->store)], new, strlen((char *)new));

	return EXIT_SUCCESS;
}

int32_t clear_player_buffer(struct player_live_record *player)
{
	memset(player->buffer, 0, BUFFER_LENGTH);
	player->loc_in_buf = NULL;

	return EXIT_SUCCESS;
}

int32_t init_player_store(struct player_live_record *player)
{
	if (player->store != NULL)
		free(player->store);

	player->store = calloc(BUFFER_LENGTH+1, sizeof(uint8_t));
	player->store_size = strlen((char *)player->buffer);
	memcpy(player->store, player->buffer, strlen((char *)player->buffer));

	return EXIT_SUCCESS;
}

int32_t clear_player_store(struct player_live_record *player)
{
	if (player->store_size == 0)
		return EXIT_SUCCESS;

	memset(player->store, '\0', player->store_size);
	free(player->store);
	player->store = NULL;
	player->store_size = 0;

	return EXIT_SUCCESS;
}

int32_t set_player_buffer_replace(struct player_live_record *player, const void *newbuf)
{
	int32_t len = strlen((char *)newbuf);
	if (len > BUFFER_LENGTH)
		len = BUFFER_LENGTH;

	player->loc_in_buf = mempcpy(player->buffer, newbuf, len);

	return EXIT_SUCCESS;
}

int32_t set_player_buffer_append(struct player_live_record *player, const void *append)
{
	if (strlen((char *)player->buffer) + strlen((char *)append) > BUFFER_LENGTH)
		return EXIT_FAILURE;

	player->loc_in_buf = mempcpy(player->loc_in_buf, append, strlen((char *)append));

	return EXIT_SUCCESS;
}

size_t get_num_of_players(void)
{
	size_t list_size = 0;
	struct player_live_record *tmp = head;

	if (tmp == NULL)
		return list_size;

	if (tmp->connected == true)
		++list_size;

	while (tmp->next != NULL) {
		if (tmp->connected == true)
			++list_size;

		tmp = tmp->next;
	}

	return list_size;
}

void reset_player_state(struct player_live_record *player)
{
	player->wait_state = NO_WAIT_STATE;
	player->holding_for_input = 0;
	clear_player_store(player);
}

int32_t store_player_id(struct player_live_record *player, const int32_t id)
{
	player->db_id = id;
	
	return EXIT_SUCCESS;
}

int32_t get_player_id(const int32_t socket)
{
	struct player_live_record *player = get_player(socket);

	return player->db_id;
}

int32_t get_socket_by_id(const int32_t id)
{
	struct player_live_record *player = get_player_by_id(id);

	return player->socket_num;
}
