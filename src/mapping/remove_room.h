#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"
#include "../mapping/room_atom.h"
#include "../mapping/room_blueprint.h"
#include "../mapping/lookup_room.h"

#include "../players/player_movement.h"

#include "../sqlite/helper_funcs.h"

int32_t remove_room(const int32_t socket);
