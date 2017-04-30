#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"
#include "../mapping/room_atom.h"
#include "../mapping/room_blueprint.h"
#include "../mapping/lookup_room.h"

#include "../players/player_movement.h"

#include "../sqlite/helper_funcs.h"
#include "../sqlite/query_matches.h"

int32_t remove_room(const int32_t socket);
int32_t remove_players_from_room(const struct coordinates coords, const int32_t r_id);
