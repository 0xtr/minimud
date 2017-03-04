#pragma once

#include "../common.h"

#include "../Players.h"
#include "../crypto/bcrypt.h"
#include "../players/player_movement.h"
#include "../players/player_details.h"
#include "../mapping/lookup_room.h"

int32_t check_for_highest_socket_num(void);
int32_t check_if_player_is_already_online(const size_t pnum);
int32_t set_player_confirm_new_pw(const int32_t socket_num, const uint8_t *command);
int32_t handle_existing_pass(const int32_t socket_num, const uint8_t *command);
int32_t handle_new_pass(const int32_t socket_num, const uint8_t *command);
