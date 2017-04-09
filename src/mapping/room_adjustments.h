#pragma once

#include "../common.h"

#include "../mapping/Coordinates.h"
#include "../mapping/RoomRecord.h"
#include "../mapping/lookup_room.h"
#include "../mapping/NewRoom.h"

#include "../players/player_movement.h"

#include "../sqlite/helper_funcs.h"

int32_t remove_players_from_room(const struct Coordinates coords);
int32_t adjust_room_flag(const int32_t socket);
int32_t adjust_room_exit(const int32_t socket, struct Coordinates destroom);
int32_t adjust_room_desc(const int32_t socket);
int32_t adjust_room_name(const int32_t socket);
