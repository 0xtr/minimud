#pragma once

#include "adjust_room_details.h"
#include "../sqlite/helper_funcs.h"
int32_t remove_room (int32_t x, int32_t y, int32_t z, const int32_t pnum);
int32_t remove_players_from_room (const int32_t x, const int32_t y, const int32_t z);
#include "remove_room.c"
