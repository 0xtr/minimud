#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"

#include "../players/player_db_record.h"

#include "../sqlite/init_db.h"

struct player_db_record *lookup_player(const uint8_t *name);
