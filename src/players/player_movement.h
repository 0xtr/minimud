#pragma once

#include "../common.h"

#include "../mapping/room_adjustments.h"
#include "../sqlite/set_get_dbs.h"
#include "../SqliteVars.h"
#include "../sqlite/helper_funcs.h"
#include "../printing/print_to_player.h"
#include "../mapping/lookup_room.h"

int32_t adjust_player_location(const int32_t socket, struct Coordinates coords);
int32_t move_player(const int32_t socket, const int32_t DIRECTION);
int32_t ensure_player_moving_valid_dir(const int32_t socket_num, const uint8_t *command);
