#pragma once

#include "../players/player_movement.h"
#include "../sqlite/helper_funcs.h"
#include "lookup_room.h"

int32_t adjust_room_details (const int32_t adjusting, const _Bool reverse, const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
int32_t get_opposite_dir (const uint8_t *dir);
int32_t remove_room (int32_t x, int32_t y, int32_t z, const int32_t pnum);
int32_t remove_players_from_room (const int32_t x, const int32_t y, const int32_t z);
int32_t insert_room (const uint8_t *rname, int32_t xloc, int32_t yloc, int32_t zloc, const uint8_t *rdesc,
                     const uint8_t *owner, const uint8_t *flags); 
