#pragma once

#include "../common.h"

#include "../mapping/room_adjustments.h"
#include "../mapping/room_atom.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/helper_funcs.h"
#include "../SqliteVars.h"

enum DB_TYPE {
	DB_ROOM,
	DB_PLAYER
};

int32_t init_dbs(void);
