#pragma once

#include "../sqlite/init_db.h"
#include "../common.h"

int32_t insert_player(struct player_live_record *player, const uint8_t *pw);
