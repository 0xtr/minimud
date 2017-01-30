#pragma once

#include "../sqlite/set_get_dbs.h"
#include "../SqliteVars.h"
#include "../sqlite/helper_funcs.h"
int32_t adjust_player_location (const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
#include "adjust_player_location.c"
