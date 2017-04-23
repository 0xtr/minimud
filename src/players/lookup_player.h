#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"
#include "../players/PlayerDBRecord.h"
#include "../sqlite/init_db.h"

struct PlayerDBRecord *lookup_player(const uint8_t *name);
