#pragma once

#include "../common.h"
#include "../items/Inventory.h"

struct player_live_record {
	int32_t db_id;

	_Bool   connected;
	_Bool   holding_for_input;
	int32_t wait_state;
	int32_t socket_num;

	uint8_t name[NAMES_MAX];
	uint8_t buffer[BUFFER_LENGTH];
	void *loc_in_buf;

	struct  sockaddr *restrict address;
	socklen_t address_len;

	uint8_t *store;
	void *loc_in_store;
	int32_t store_size;

	struct player_live_record *prev;
	struct player_live_record *next;
	struct Inventory *inventory;
};

struct player_live_record *get_player_head(void);
struct player_live_record *get_player(const int32_t socket);
struct player_live_record *get_player_by_index(const size_t index);
struct player_live_record *get_player_by_socket(const int32_t socket);
struct player_live_record *get_player_by_id(const int32_t id);

int32_t get_player_id(const int32_t socket);
int32_t get_socket_by_id(const int32_t id);

int32_t add_new_player(const int32_t socket);
int32_t remove_last_player_record(void);
int32_t remove_player_by_socket(const int32_t socket);

int32_t store_player_id(struct player_live_record *player, const int32_t id);
size_t get_num_of_players(void);
int32_t clear_player_store(struct player_live_record *player);
int32_t clear_player_buffer(struct player_live_record *player);
int32_t init_player_store(struct player_live_record *player);
int32_t set_player_name(struct player_live_record *player, const uint8_t *name);
int32_t set_player_wait_state(struct player_live_record *player, const int32_t wait_state);
int32_t set_player_holding_for_input(struct player_live_record *player, const _Bool hold);
int32_t set_player_buffer_replace(struct player_live_record *player, const void *newbuf);
int32_t set_player_buffer_append(struct player_live_record *player, const void *append);
int32_t set_player_store_replace(struct player_live_record *player, const void *newval);
int32_t set_player_store_append(struct player_live_record *player, const void *append);

void reset_player_state(struct player_live_record *player);
