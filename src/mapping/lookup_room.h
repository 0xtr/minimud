#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"
#include "../mapping/room_atom.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/helper_funcs.h"

#include "../util/outgoing_handler.h"

struct room_atom *lookup_room(const struct coordinates coords);
struct room_atom *lookup_room_by_id(const int32_t id);
int32_t lookup_room_exits(const struct coordinates origin, struct room_atom *dest_room);
int32_t lookup_room_name_from_coords(struct player_live_record *player, const struct coordinates coords);
int32_t compare_room_owner(struct player_live_record *player, const struct coordinates coords);
int32_t has_exit_for_dir(struct room_atom *origin, struct room_atom *dest);
