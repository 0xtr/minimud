#pragma once

#include "../common.h"

#include "../sqlite/init_db.h"
#include "../players/PlayerDBRecord.h"

struct PlayerDBRecord *lookup_player(const uint8_t *name);
