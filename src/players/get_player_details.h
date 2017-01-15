#pragma once

#include "../sqlite/set_get_dbs.h"
int32_t get_existing_player_hash (const int32_t pnum);
int32_t get_player_coord (int32_t coord_type, int32_t pnum);
int32_t get_next_player_num (void);
#include "get_player_details.c"
