#pragma once

#include "../sqlite/set_get_dbs.h"
#include "../sqlite/callback.h"
#include "../sqlite_vars.h"
int32_t init_db (const int DB_TYPE);
void reset_player_sqlite_vars (void);
#include "init_db.c"
