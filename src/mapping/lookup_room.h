#pragma once

#include "../util/send_and_ensure.h"
#include "../sqlite/callback.h"
#include "map_vars.h"
int32_t lookup_room (int32_t x, int32_t y, int32_t z, int32_t socknum); 
#include "lookup_room.c"
