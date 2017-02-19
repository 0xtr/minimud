#pragma once

#include "../common.h"

sqlite3 *get_roomdb (void);
void set_roomdb (sqlite3 *db);
sqlite3 *get_playerdb (void);
void set_playerdb (sqlite3 *db);
