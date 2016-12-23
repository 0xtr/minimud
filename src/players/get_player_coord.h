#pragma once

#include "../sqlite/callback.h"
#include "../sqlite_vars.h"
#include "../sqlite/init_db.h"
int32_t get_player_coord (int32_t coord_type, int32_t pnum);
#include "get_player_coord.c"
