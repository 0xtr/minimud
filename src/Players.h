#pragma once

Player* get_player (const int32_t pnum);
void remove_player_record (const int32_t pnum);
struct Player *get_new_player ();
struct Player *get_newest_player ();
struct sockaddr *restrict get_newest_player_address ();
socklen_t *get_newest_player_address_len ();
int32_t getplayernum (const int32_t socknum);
int32_t get_num_of_players ();
int32_t get_player_socket (const int32_t pnum);
uint8_t *get_player_buffer (const int32_t pnum);
_Bool get_player_in_use (const int32_t pnum);
_Bool get_player_hold_for_input (const int32_t pnum);
int32_t get_player_wait_state (const int32_t pnum);
void clear_player_store (const int32_t pnum);
void init_player_store (const int32_t pnum);
void set_player_pname (const int32_t pnum, const uint8_t *name);
uint8_t *get_player_pname (const int32_t pnum);
