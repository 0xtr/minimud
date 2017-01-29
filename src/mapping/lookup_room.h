#pragma once

#include "../sqlite/set_get_dbs.h"
#include "../Map.h"
#include "../util/outgoing_msg_handler.h"
#include "../sqlite/helper_funcs.h"
_Bool is_vector_west (const int32_t x, const int32_t y);
_Bool has_west_exit (const Map *map);
Map *lookup_room (const int32_t x, const int32_t y, const int32_t z, const int32_t pnum);
int32_t lookup_room_exits (const int32_t pnum, const int32_t xadj, const int32_t yadj, const int32_t zadj);
int32_t lookup_room_name_from_coords (const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
int32_t compare_room_owner (const int32_t pnum, const uint8_t *x, const uint8_t *y, const uint8_t *z);
#include "lookup_room.c"
