#pragma once

#include "../common.h"

#include "../commands/classify_cmd.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/sqlite_callbacks.h"

#include "../printing/printing.h"

#include "../rooms/room_actions.h"

int32_t adjust_player_location(struct player_live_record *player, const int32_t rid);
int32_t move_player(struct player_live_record *player, const int32_t DIRECTION);
int32_t ensure_player_moving_valid_dir(struct player_live_record *player, 
		const uint8_t *command);
