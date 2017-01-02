#pragma once

#include "../Players.h"
#include "../files/test_macros.c"
#include "../files/log_issue.h"
int32_t send_and_ensure (const int32_t player_num, const uint8_t *generic_buf);
#include "send_and_ensure.c"
