#pragma once

#include "../Players.h"
#include <sys/select.h>
#include "../util/process_data.h"
int32_t check_if_data_is_waiting (const fd_set rfds);
#include "check_if_data_is_waiting.c"
