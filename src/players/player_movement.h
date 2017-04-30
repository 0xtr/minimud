#pragma once

#include "../common.h"

#include "../commands/classify_cmd.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/helper_funcs.h"

#include "../printing/printing.h"

#include "../mapping/lookup_room.h"
#include "../mapping/alterations.h"

int32_t adjust_player_location(const int32_t socket, struct coordinates coords);
int32_t move_player(const int32_t socket, const int32_t DIRECTION);
int32_t ensure_player_moving_valid_dir(const int32_t socket, const uint8_t *command);
