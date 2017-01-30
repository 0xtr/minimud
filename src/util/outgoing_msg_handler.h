#pragma once

#include <math.h>
#include "../util/shutdown_socket.h"
#include "../Players.h"
#include "../files/test_macros.c"
int32_t outgoing_msg_handler (const int32_t player_num);
#define CHARS_FOR_PROMPT_AND_NULL 2
#include "outgoing_msg_handler.c"
