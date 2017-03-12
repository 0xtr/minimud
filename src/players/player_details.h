#pragma once

#define SALTLEN   50
#include "../common.h"

#include "../printing/print_to_player.h"
#include "../sqlite/helper_funcs.h"
#include "../Players.h"
#include "../sqlite/set_get_dbs.h"
#include "../SqliteVars.h"
#include "../crypto/bcrypt.h"
#include "../crypto/explicit_bzero.h"

int32_t insert_player(const uint8_t *pname, const uint8_t *pw, const int32_t socknum); 
int32_t get_existing_player_hash(const int32_t socket);
int32_t get_player_coord(int32_t coord_type, int32_t socket);
int32_t get_next_player_num(void);
int32_t lookup_player(const uint8_t *pname);
