#include "common.h"
#include "Players.h"

static struct Player *head = NULL;
static struct Player *tail = NULL;

#define find_player_node \
	struct Player *curr = head;\
	while ((curr = curr->next) != NULL) {\
		if (curr->socket_num == socket)\
			break;\
	}\

#define player_not_found\
	curr->socket_num != socket || curr == NULL

struct Player *get_player_head(void)
{
	return head;
}

int32_t remove_last_player_record(void)
{
	if (tail == NULL)
		return EXIT_FAILURE;

	struct Player *curr = tail;

	curr = tail->prev;
	free(tail);

	curr->next = NULL;
	tail = curr;

	return EXIT_SUCCESS;
}

int32_t remove_player_by_socket(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	struct Player *prev = curr->prev;
	struct Player *next = curr->next;

	free(curr);
	prev->next = next;
	next->prev = prev;

	return EXIT_SUCCESS;
}

int32_t add_new_player(const int32_t socket)
{
	struct Player *curr = (struct Player *)malloc(sizeof(struct Player));
	if (curr == NULL)
		return EXIT_FAILURE;

	memset(curr, 0, sizeof(struct Player));

    	curr->socket_num = socket;
    	curr->inventory = get_new_player_inventory(socket);
	curr->holding_for_input = true;
	curr->wait_state = THEIR_NAME;
	curr->prev = tail;

	tail->next = curr;
	tail = curr;

	return EXIT_SUCCESS;
}

uint8_t *get_player_store(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return NULL;

	return curr->store;
}

int32_t set_player_holding_for_input(const int32_t socket, const _Bool hold)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	curr->holding_for_input = hold;

	return EXIT_SUCCESS;
}

int32_t get_player_wait_state(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return -1;

	return curr->wait_state;
}

int32_t set_player_wait_state(const int32_t socket, const int32_t wait_state)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	curr->wait_state = wait_state;

	return EXIT_SUCCESS;
}

int32_t set_player_name(const int32_t socket, const uint8_t *name)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	strncpy((char *)curr->name, (char *)name, NAMES_MAX);

	return EXIT_SUCCESS;
}

int32_t set_player_store_replace(const int32_t socket, const uint8_t *newval)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	strncpy((char *)curr->store, (char *)newval, BUFFER_LENGTH);

	return EXIT_SUCCESS;
}

int32_t set_player_store_append(const int32_t socket, const uint8_t *new)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	strncat((char *)curr->store, (char *)new, BUFFER_LENGTH 
			- strlen((char *)curr->store));

	return EXIT_SUCCESS;
}

int32_t clear_player_buffer(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	memset(curr->buffer, 0, BUFFER_LENGTH);

	return EXIT_SUCCESS;
}

uint8_t *get_player_name(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return NULL;

	return curr->name;
}

int32_t init_player_store(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	curr->store = calloc(BUFFER_LENGTH, sizeof(uint8_t));
	curr->store_size = strlen((char *)curr->buffer);
	strncpy((char *)curr->store, (char *)curr->buffer, 
			strlen((char *)curr->buffer));

	return EXIT_SUCCESS;
}

int32_t clear_player_store(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	assert(curr->store_size != 0);
	memset(curr->store, '\0', curr->store_size);
	free(curr->store);
	curr->store = NULL;
	curr->store_size = 0;

	return EXIT_SUCCESS;
}

_Bool get_player_holding_for_input(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return NULL;

	return curr->holding_for_input;
}

struct sockaddr *get_player_address(const int32_t socket)
{
	find_player_node;
// trying to pass it an arg with void

	if (player_not_found)
		return NULL;

	return curr->address;
}

uint8_t *get_player_buffer(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return NULL;

	return curr->buffer;
}

socklen_t *get_player_address_len(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return 0;

	return &curr->address_len;
}

int32_t set_player_buffer_replace(const int32_t socket, const uint8_t *newbuf)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	strncpy((char *)curr->buffer, (char *)newbuf, BUFFER_LENGTH);

	return EXIT_SUCCESS;
}

int32_t set_player_buffer_append(const int32_t socket, const uint8_t *append)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	strncat((char *)curr->buffer, (char *)append, 
		strlen((char *)curr->buffer) - strlen((char *)append));

	return EXIT_SUCCESS;
}

size_t get_num_of_players(void)
{
	size_t list_size = 0;
	struct Player *tmp = head;
	for (list_size = 0; (tmp = tmp->next) != NULL; ++list_size) {
	}
	return list_size;
}
