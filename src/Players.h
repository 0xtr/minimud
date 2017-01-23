#pragma once

#define  NAMES_MIN        5
#define  NAMES_MAX        45
#include <stdbool.h>
struct Player_l {
    _Bool   in_use;
    _Bool   hold_for_input;
    int32_t wait_state;
    int32_t socket_num;
    uint8_t pname[NAMES_MAX];
    uint8_t holder[NAMES_MAX];
    struct  sockaddr *restrict address;
    socklen_t address_len;
    uint8_t buffer[BUFFER_LENGTH];
    uint8_t *store;
    int32_t store_size;
    struct  Player* prev;
    struct  Player* next;
};
typedef struct Player_l Player;
struct Player *get_player (const int32_t pnum);
struct Player *get_new_player ();
struct Player *get_newest_player ();
struct sockaddr *restrict get_newest_player_address ();
socklen_t *get_newest_player_address_len ();
void remove_player_record (const int32_t pnum);
int32_t get_player_wait_state (const int32_t pnum);
int32_t getplayernum (const int32_t socknum);
int32_t get_num_of_players ();
int32_t get_player_socket (const int32_t pnum);
_Bool get_player_in_use (const int32_t pnum);
_Bool get_player_hold_for_input (const int32_t pnum);
uint8_t *get_player_buffer (const int32_t pnum);
uint8_t *get_player_pname (const int32_t pnum);
uint8_t *get_player_store (const int32_t pnum);
void clear_player_store (const int32_t pnum);
void clear_player_buffer (const int32_t pnum);
void init_player_store (const int32_t pnum);
void set_player_pname (const int32_t pnum, const uint8_t *name);
void set_player_wait_state (const int32_t pnum, const int32_t wait_state);
void set_player_hold_for_input (const int32_t pnum, const _Bool hold);
void set_player_buffer_replace (const int32_t pnum, const uint8_t *newbuf);
void set_player_buffer_append (const int32_t pnum, const uint8_t *append);
void set_player_store_replace (const int32_t pnum, const uint8_t *newval);
void set_player_store_append (const int32_t pnum, const uint8_t *append);
