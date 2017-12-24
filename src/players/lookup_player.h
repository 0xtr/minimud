#pragma once

#include "../common.h"

#include "../players/player_db_record.h"

#include "../sqlite/sqlite_init_dbs.h"

struct player_db_record *lookup_player(const uint8_t *name);
