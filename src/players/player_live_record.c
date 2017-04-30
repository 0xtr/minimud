#include "player_live_record.h"

static struct player_live_record *head = NULL;
static struct player_live_record *tail = NULL;

#define find_player_node \
	struct player_live_record *curr = head;\
	if (curr != NULL) {\
		while (curr != NULL) {\
			if (curr->socket_num == socket)\
				break;\
			curr = curr->next;\
		}\
	}
// make generic
#define find_player_node_by_id \
	struct player_live_record *curr = head;\
	if (curr != NULL) {\
		while (curr != NULL) {\
			if (curr->db_id == id)\
				break;\
			curr = curr->next;\
		}\
	}

#define player_not_found\
	curr == NULL || curr->socket_num != socket 

#define player_not_found_by_id\
	curr == NULL || curr->db_id != id 

struct player_live_record *get_player_head(void)
{
	return head;
}

struct player_live_record *get_player(const size_t index)
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

struct player_live_record *get_player_by_socket(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return NULL;

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
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

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
	struct player_live_record *curr = (struct player_live_record *)malloc(sizeof(struct player_live_record));
	if (curr == NULL)
		return EXIT_FAILURE;

	memset(curr, 0, sizeof(struct player_live_record));

    	curr->socket_num = socket;
    	curr->inventory = get_new_player_inventory(socket);
	curr->holding_for_input = true;
	curr->wait_state = THEIR_NAME;
	curr->prev = tail;
	memset(curr->buffer, 0, BUFFER_LENGTH);

	if (head == NULL)
		head = curr;
	if (tail != NULL)
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

	memcpy(curr->name, name, NAMES_MAX);

	return EXIT_SUCCESS;
}

int32_t set_player_store_replace(const int32_t socket, const void *newval)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	memset(curr->store, '\0', curr->store_size);
	int32_t len = strlen((char *)newval);
	if (len > BUFFER_LENGTH)
		len = BUFFER_LENGTH;

	memcpy(curr->store, newval, len);
	curr->store_size = len;

	return EXIT_SUCCESS;
}

int32_t set_player_store_append(const int32_t socket, const void *new)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	if (strlen((char *)curr->store) + strlen((char *)new) > BUFFER_LENGTH)
		return EXIT_FAILURE;

	memcpy(&curr->store[strlen((char *)curr->store)], new, strlen((char *)new));

	return EXIT_SUCCESS;
}

int32_t clear_player_buffer(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	memset(curr->buffer, 0, BUFFER_LENGTH);
	curr->loc_in_buf = NULL;

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

	if (curr->store != NULL)
		free(curr->store);

	curr->store = calloc(BUFFER_LENGTH+1, sizeof(uint8_t));
	curr->store_size = strlen((char *)curr->buffer);
	memcpy(curr->store, curr->buffer, strlen((char *)curr->buffer));

	return EXIT_SUCCESS;
}

int32_t clear_player_store(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	if (curr->store_size == 0)
		return EXIT_SUCCESS;

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

int32_t set_player_buffer_replace(const int32_t socket, const void *newbuf)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	clear_player_buffer(socket);

	int32_t len = strlen((char *)newbuf);
	if (len > BUFFER_LENGTH)
		len = BUFFER_LENGTH;

	curr->loc_in_buf = mempcpy(curr->buffer, newbuf, len);

	return EXIT_SUCCESS;
}

int32_t set_player_buffer_append(const int32_t socket, const void *append)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	if (strlen((char *)curr->buffer) + strlen((char *)append) > BUFFER_LENGTH)
		return EXIT_FAILURE;

	curr->loc_in_buf = mempcpy(curr->loc_in_buf, append, strlen((char *)append));

	return EXIT_SUCCESS;
}

size_t get_num_of_players(void)
{
	size_t list_size = 0;
	struct player_live_record *tmp = head;

	if (tmp == NULL)
		return list_size;

	for (list_size = 0; (tmp = tmp->next) != NULL; ++list_size) { } 

	return list_size;
}

void reset_player_state(const int32_t socket)
{
	set_player_wait_state(socket, NO_WAIT_STATE);
	set_player_holding_for_input(socket, 0);
	clear_player_store(socket);
}

int32_t store_player_id(const int32_t socket, const int32_t id)
{
	find_player_node;

	if (player_not_found)
		return EXIT_FAILURE;

	curr->db_id = id;
	
	return EXIT_SUCCESS;
}

int32_t get_player_id(const int32_t socket)
{
	find_player_node;

	if (player_not_found)
		return -1;

	return curr->db_id;
}

int32_t get_socket_by_id(const int32_t id)
{
	find_player_node_by_id;

	if (player_not_found_by_id)
		return -1;

	return curr->socket_num;
}
