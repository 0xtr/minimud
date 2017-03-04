#pragma once

#include "Inventory.h"

struct Player {
    _Bool   holding_for_input;
    int32_t wait_state;
    int32_t socket_num;

    uint8_t name[NAMES_MAX];
    uint8_t buffer[BUFFER_LENGTH];

    struct  sockaddr *restrict address;
    socklen_t address_len;

    uint8_t *store;
    int32_t store_size;
    struct  Player *prev;
    struct  Player *next;

    struct Inventory *inventory;
};

struct Player *get_player_head(void);
int32_t add_new_player(const int32_t socket);
int32_t remove_last_player_record(void);
int32_t remove_player_by_socket(const int32_t socket);

struct sockaddr *get_player_address(const int32_t socket);
socklen_t *get_player_address_len(const int32_t socket);
int32_t get_player_wait_state(const int32_t socket);
size_t get_num_of_players(void);
int32_t get_player_socket(const int32_t socket);
_Bool get_player_holding_for_input(const int32_t socket);
uint8_t *get_player_buffer(const int32_t socket);
uint8_t *get_player_name(const int32_t socket);
uint8_t *get_player_store(const int32_t socket);
int32_t clear_player_store(const int32_t socket);
int32_t clear_player_buffer(const int32_t socket);
int32_t init_player_store(const int32_t socket);
int32_t set_player_name(const int32_t socket, const uint8_t *name);
int32_t set_player_wait_state(const int32_t socket, const int32_t wait_state);
int32_t set_player_holding_for_input(const int32_t socket, const _Bool hold);
int32_t set_player_buffer_replace(const int32_t socket, const uint8_t *newbuf);
int32_t set_player_buffer_append(const int32_t socket, const uint8_t *append);
int32_t set_player_store_replace(const int32_t socket, const uint8_t *newval);
int32_t set_player_store_append(const int32_t socket, const uint8_t *append);
