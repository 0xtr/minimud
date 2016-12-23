#pragma once

#include "map_vars.h"
#include "../sqlite/callback.h"
int32_t insert_room (const uint8_t *rname, int32_t xloc, int32_t yloc, int32_t zloc, const uint8_t *rdesc,
                        const uint8_t *owner, const uint8_t *flags); 
#include "insert_room.c"
