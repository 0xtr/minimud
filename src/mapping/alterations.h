#pragma once

#include "../common.h"

#include "../sqlite/helper_funcs.h"
#include "../sqlite/init_db.h"
#include "../sqlite/run_sql.h"

#include "../mapping/coordinates.h"
#include "../mapping/room_atom.h"
#include "../mapping/room_blueprint.h"
#include "../mapping/lookup_room.h"

#include "../players/player_movement.h"

int32_t adjust_room_flag(const int32_t socket);
int32_t adjust_room_desc(const int32_t socket);
int32_t adjust_room_name(const int32_t socket);
int32_t link_rooms(const int32_t dir, struct room_atom *existing, struct room_atom *newroom);
int32_t unlink_rooms(const int32_t dir, struct room_atom *existing, struct room_atom *newroom);
