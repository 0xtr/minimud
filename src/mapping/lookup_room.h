#pragma once

#include "../common.h"

#include "../sqlite/set_get_dbs.h"
#include "../mapping/RoomRecord.h"
#include "../util/outgoing_handler.h"
#include "../sqlite/helper_funcs.h"

struct RoomRecord *lookup_room(const int32_t x, const int32_t y, const int32_t z, const int32_t socket);
int32_t lookup_room_exits(const int32_t socket, const int32_t xadj, const int32_t yadj, const int32_t zadj);
int32_t lookup_room_name_from_coords(const int32_t socket, const int32_t x, const int32_t y, const int32_t z);
int32_t compare_room_owner(const int32_t socket, const int32_t x, const int32_t y, const int32_t z);
int32_t has_exit_for_dir(const int32_t x, const int32_t y, const int32_t z, const struct RoomRecord *map);
