#pragma once

#include "../common.h"
#include "../sqlite/init_db.h"
#include "../mapping/room_adjustments.h"
#include "../mapping/room_atom.h"

struct room_atom *insert_room(struct room_blueprint rconfig);
