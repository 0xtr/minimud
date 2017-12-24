#pragma once

#include "../common.h"

#include "../rooms/room_actions.h"
#include "../rooms/room_db_record.h"

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/sqlite_callbacks.h"
#include "../sqlite_vars.h"

enum DB_TYPE {
	DB_ROOM,
	DB_ROOM_COUNT,
	DB_PLAYER,
	DB_PLAYER_COUNT,
	DB_OBJECT,
	DB_OBJECT_COUNT
};

int32_t init_dbs(void);
