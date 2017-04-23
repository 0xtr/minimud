#pragma once

#include "../common.h"

#include "../sqlite/init_db.h"

int32_t run_sql(char *query, void *data, const int32_t type);
