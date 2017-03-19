#pragma once

#include "../common.h"

#include "../players/PlayerDBRecord.h"
#include "../players/PlayerLiveRecord.h"
#include "../printing/print_to_player.h"
#include "../sqlite/helper_funcs.h"
#include "../sqlite/set_get_dbs.h"
#include "../SqliteVars.h"
#include "../crypto/bcrypt.h"
#include "../crypto/explicit_bzero.h"

int32_t get_player_coord(int32_t coord_type, int32_t socket);
int32_t get_next_player_num(void);
struct PlayerDBRecord *lookup_player(const uint8_t *name);
