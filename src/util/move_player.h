#pragma once

#include "../printing/print_output.h"
#include "../players/adjust_player_location.h"
#include "../mapping/lookup_room_exits.h"
int32_t move_player (int32_t socknum, uint8_t command[]);
#include "move_player.c"
