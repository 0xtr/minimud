#pragma once

#include "../common.h"

#include "../players/player_db_record.h"
#include "../players/player_live_record.h"

#include "../printing/printing.h"

#include "../sqlite/helper_funcs.h"
#include "../sqlite/set_get_dbs.h"

#include "../crypto/bcrypt.h"

#include "../bsd/explicit_bzero.h"

struct coordinates get_player_coords(struct player_live_record *player);
int32_t get_next_player_num(void);
struct player_db_record *lookup_player(const uint8_t *name);
int32_t get_player_loc_id(struct player_live_record *player);
