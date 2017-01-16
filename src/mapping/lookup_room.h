#pragma once

#include "../Map.h"
#include "../util/send_and_ensure.h"
#include "../sqlite/helper_funcs.h"
_Bool is_vector_west (const int32_t x, const int32_t y);
_Bool has_west_exit (const Map map);
Map *lookup_room (const int32_t x, const int32_t y, const int32_t z, const int32_t socknum);
int32_t lookup_room_exits (const int32_t pnum, const int32_t xadj, const int32_t yadj, const int32_t zadj);
int32_t lookup_room_name_from_coords (const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
#include "lookup_room.c"
