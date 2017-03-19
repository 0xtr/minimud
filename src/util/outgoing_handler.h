#pragma once

#define CHARS_FOR_PROMPT_AND_NULL 3
#include "../common.h"

#include "../players/PlayerLiveRecord.h"
#include "incoming_handler.h"

int32_t outgoing_handler(const int32_t socket);
