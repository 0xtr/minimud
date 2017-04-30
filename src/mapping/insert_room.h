#pragma once

#include "../common.h"
#include "../sqlite/init_db.h"
#include "../mapping/alterations.h"
#include "../mapping/room_atom.h"

struct room_atom *insert_room(struct room_blueprint rconfig);
