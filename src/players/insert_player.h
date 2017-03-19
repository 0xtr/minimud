#pragma once

#include "../sqlite/init_db.h"
#include "../common.h"

int32_t insert_player(const uint8_t *pname, const uint8_t *pw, const int32_t socknum); 
