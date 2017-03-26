#pragma once

#include "../common.h"

#include "../mapping/Coordinates.h"
#include "../mapping/RoomRecord.h"
#include "../mapping/lookup_room.h"
#include "../mapping/NewRoom.h"

#include "../players/player_movement.h"

#include "../sqlite/helper_funcs.h"

int32_t remove_room(const int32_t socket);
