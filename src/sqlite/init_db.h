#pragma once

#include "../common.h"

#include "../sqlite/set_get_dbs.h"
#include "../mapping/room_adjustments.h"
#include "../sqlite/helper_funcs.h"
#include "../SqliteVars.h"

int32_t init_db(const int32_t DB_TYPE);
