#pragma once

#include "../common.h"

#include "../players/PlayerDBRecord.h"
#include "../players/PlayerLiveRecord.h"

#include "../printing/printing.h"

#include "../sqlite/helper_funcs.h"
#include "../sqlite/set_get_dbs.h"
#include "../SqliteVars.h"

#include "../crypto/bcrypt.h"

#include "../bsd/explicit_bzero.h"

struct Coordinates get_player_coords(const int32_t socket);
int32_t get_next_player_num(void);
struct PlayerDBRecord *lookup_player(const uint8_t *name);
