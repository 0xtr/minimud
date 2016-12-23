#pragma once

#include "../sqlite_vars.h"
int callback (void *NotUsed __attribute__ ((unused)), int argc, char **argv, char **azColName __attribute__ ((unused)));
#include "callback.c"
