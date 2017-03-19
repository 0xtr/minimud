#pragma once

#include "../common.h"

#include "../mapping/Coordinates.h"
#include "../players/PlayerDBRecord.h"
#include "../sqlite/init_db.h"

struct PlayerDBRecord *lookup_player(const uint8_t *name);
