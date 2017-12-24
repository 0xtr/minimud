#pragma once

#include "../common.h"

#include "sqlite_init_dbs.h"

int32_t run_sql(char *query, void *data, const int32_t type);
