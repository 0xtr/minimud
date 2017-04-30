#pragma once

#include "../common.h"

#include "../mapping/alterations.h"
#include "../mapping/room_atom.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/helper_funcs.h"
#include "../sqlite_vars.h"

enum DB_TYPE {
	DB_ROOM,
	DB_ROOM_COUNT,
	DB_PLAYER,
	DB_PLAYER_COUNT
};

int32_t init_dbs(void);
