#pragma once

#include "../util/send_and_ensure.h"
#include "../players/get_player_coord.h"
#include "../sqlite/callback.h"
#include "map_vars.h"
int32_t lookup_room_exits (int32_t pnum, int32_t xadj, int yadj, int zadj); 
#include "lookup_room_exits.c"
