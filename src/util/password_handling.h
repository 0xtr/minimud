#pragma once

#include "../crypto/bcrypt.h"
#include "../players/adjust_player_location.h"
#include "../players/insert_player.h"
#include "../mapping/lookup_room.h"
int32_t set_player_confirm_new_pw (const int32_t socket_num, const uint8_t *command);
int32_t handle_existing_pass (const int32_t socket_num, const uint8_t *command);
int32_t handle_new_pass (const int32_t socket_num, const uint8_t *command);
#include "password_handling.c"
