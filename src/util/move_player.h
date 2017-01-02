#pragma once

#include "../printing/print_output.h"
#include "../players/adjust_player_location.h"
#include "../mapping/lookup_room.h"
int32_t move_player (const int32_t socknum, const uint8_t command[]);
#include "move_player.c"
