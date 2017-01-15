#pragma once

#define SALTLEN   50
#include "../SqliteVars.h"
#include "../crypto/bcrypt.h"
#include <bsd/stdlib.h>
int32_t insert_player (const uint8_t *pname, const uint8_t *pw, const int32_t socknum); 
#include "insert_player.c"
