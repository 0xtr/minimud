#pragma once

#include "../common.h"

#include "../sqlite/sqlite_init_dbs.h"

int32_t insert_player(struct player_live_record *player, const uint8_t *pw);
