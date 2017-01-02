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
