#include "common.h"
#include "Players.h"

static Player_struct *player;
static Player_struct *curr, *head;

Player_struct *get_player(const int32_t pnum)
{
	return &player[pnum];
}

Player_struct *get_newest_player(void)
{
	return curr;
}

uint8_t *get_player_store(const int32_t pnum)
{
	return player[pnum].store;
}

void set_player_hold_for_input(const int32_t pnum, const _Bool hold)
{
	player[pnum].hold_for_input = hold;
}

int32_t get_player_wait_state(const int32_t pnum)
{
	return player[pnum].wait_state;
}

void set_player_wait_state(const int32_t pnum, const int32_t wait_state)
{
	player[pnum].wait_state = wait_state;
}

void set_player_pname(const int32_t pnum, const uint8_t *name)
{
	strncpy((char *)player[pnum].pname, (char *)name, NAMES_MAX);
}

void set_player_store_replace(const int32_t pnum, const uint8_t *newval)
{
	strncpy((char *)player[pnum].store, (char *)newval, BUFFER_LENGTH);
}

void set_player_store_append(const int32_t pnum, const uint8_t *new)
{
	strncat((char *)player[pnum].store, (char *)new, BUFFER_LENGTH 
			- strlen((char *)player[pnum].store));
}

void clear_player_buffer(const int32_t pnum)
{
	memset(player[pnum].buffer, 0, BUFFER_LENGTH);
}

uint8_t *get_player_pname(const int32_t pnum)
{
	return player[pnum].pname;
}

void init_player_store(const int32_t pnum)
{
	player[pnum].store = calloc(BUFFER_LENGTH, sizeof(uint8_t));
	player[pnum].store_size = strlen((char *)player[pnum].buffer);
	strncpy((char *)player[pnum].store, (char *)player[pnum].buffer, 
			strlen((char *)player[pnum].buffer));
}

void clear_player_store(const int32_t pnum)
{
	assert(player[pnum].store_size != 0);
	memset(player[pnum].store, '\0', player[pnum].store_size);
	free(player[pnum].store);
	player[pnum].store = NULL;
	player[pnum].store_size = 0;
}

_Bool get_player_hold_for_input(const int32_t pnum)
{
	return player[pnum].hold_for_input;
}

struct sockaddr *get_newest_player_address(void)
{
	return curr->address;
}

uint8_t *get_player_buffer(const int32_t pnum)
{
	return player[pnum].buffer;
}

_Bool get_player_in_use(const int32_t pnum)
{
	return player[pnum].in_use;
}

void set_player_in_use(const int32_t pnum, _Bool in_use)
{
	player[pnum].in_use = in_use;
}

int32_t get_player_socket(const int32_t pnum)
{
	return player[pnum].socket_num;
}

void set_player_socket(const int32_t pnum, const int32_t socket)
{
	player[pnum].socket_num = socket;
}

socklen_t *get_newest_player_address_len(void)
{
	return &curr->address_len;
}

void set_player_buffer_replace(const int32_t pnum, const uint8_t *newbuf)
{
	strncpy((char *)player[pnum].buffer, (char *)newbuf, BUFFER_LENGTH);
}

void set_player_buffer_append(const int32_t pnum, const uint8_t *append)
{
	strncat((char *)player[pnum].buffer, (char *)append, 
		strlen((char *)player[pnum].buffer) - strlen((char *)append));
}

void remove_player_record(const int32_t pnum)
{
	// remove prev and next and set surrounding records to each other
}

void remove_last_player_record(void)
{
	printf("players taking %lu bytes space\n", sizeof(player));
	free(head);
	head = curr->prev;
	head->next = NULL;
	head = curr;
	printf("players now taking %lu bytes space\n", sizeof(player));
}

Player_struct *get_new_player(void)
{
	printf("players taking %lu bytes space\n", sizeof(player));
	curr = (Player_struct *)malloc(sizeof(Player_struct));
	curr->prev = head;
	head->next = curr;
	head = curr;
	printf("players now taking %lu bytes space\n", sizeof(player));
	return head;
}

size_t get_num_of_players(void)
{
	size_t list_size = 0;
	Player_struct *tmp = head;
	for (list_size = 0; (tmp = tmp->prev) != NULL; ++list_size) {
	}
	return list_size;
}

int32_t getplayernum(const int32_t socknum)
{
	int32_t i;
	_Bool found = false;
	Player_struct *tmp = head;
	for (i = 0; tmp->prev != NULL; ++i) {
		if (player[i].socket_num == socknum) {
			found = true;
			break;
		}
		tmp = tmp->prev;
	}
	if (found == 1) {
		return i;
	} else {
		return -1; 
	}
}
