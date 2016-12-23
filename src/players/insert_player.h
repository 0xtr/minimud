#pragma once

#include "../sqlite_vars.h"
#include <bsd/stdlib.h>
#include "get_next_player_num.h"
int32_t insert_player (const uint8_t *pname, const uint8_t *pw, int32_t socknum); 
#include "insert_player.c"
