#pragma once

#include "../common.h"

#include "../rooms/room_actions.h"

#include "../players/insert_player.h"
#include "../players/player_live_record.h"
#include "../players/player_db_record.h"
#include "../players/player_movement.h"
#include "../players/player_details.h"

#include "../crypto/bcrypt.h"

int32_t check_for_highest_socket_num(void);
int32_t set_player_confirm_new_pw(struct player_live_record *player, const uint8_t *command);

int32_t handle_existing_pass(struct player_live_record *player, const uint8_t *command);
int32_t handle_new_pass(struct player_live_record *player, const uint8_t *command);

_Bool check_if_player_is_already_online(struct player_live_record *player, const uint8_t *name);
_Bool check_if_name_is_valid(struct player_live_record *player, const uint8_t *name);
_Bool check_if_name_is_reserved(struct player_live_record *player, const uint8_t *name);

