#pragma once

#include "lookup_room.h"
#include "../sqlite/helper_funcs.h"
int32_t adjust_room_details (const int32_t adjusting, const _Bool reverse, const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
int32_t get_opposite_dir (const uint8_t *dir);
#include "adjust_room_details.c"
