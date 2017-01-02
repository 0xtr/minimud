#pragma once

#include "../SqliteVars.h"
int callback (void *NotUsed __attribute__ ((unused)), int argc, char **argv, char **azColName __attribute__ ((unused)));
int32_t get_sqlite_rows_count ();
void increment_sqlite_rows_count ();
void set_sqlite_rows_count (const int32_t newval);
void reset_sqlite_rows_count ();
#include "helper_funcs.c"
