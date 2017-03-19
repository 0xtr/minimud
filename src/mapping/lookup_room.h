#pragma once

#include "../common.h"

#include "../mapping/Coordinates.h"
#include "../mapping/RoomRecord.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/helper_funcs.h"

#include "../util/outgoing_handler.h"

struct RoomRecord *lookup_room(const struct Coordinates coords);
int32_t lookup_room_exits(const int32_t socket, const struct Coordinates coords);
int32_t lookup_room_name_from_coords(const int32_t socket, const struct Coordinates coords);
int32_t compare_room_owner(const int32_t socket, const struct Coordinates coords);
int32_t has_exit_for_dir(const struct Coordinates coords, const struct RoomRecord *map);
