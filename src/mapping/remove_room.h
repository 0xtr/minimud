#pragma once

#include "adjust_room_details.h"
#include "remove_players_from_room.h"
#include "../sqlite/callback.h"
#include "map_vars.h"
int32_t remove_room (int32_t x, int32_t y, int32_t z, int32_t socknum); 
#include "remove_room.c"
