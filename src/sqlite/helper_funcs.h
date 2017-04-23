#pragma once

#include "../common.h"

#include "../players/PlayerDBRecord.h"
#include "../mapping/room_atom.h"
#include "../SqliteVars.h"

int32_t get_sqlite_rows_count(void);
void increment_sqlite_rows_count(void);
void set_sqlite_rows_count(const int32_t newval);
void reset_sqlite_rows_count(void);
int room_callback (void *data, int argc, char **argv, char **azColName);
int player_callback (void *data, int argc, char **argv, char **azColName);
