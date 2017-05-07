#pragma once

#define CHARS_FOR_PROMPT_AND_NULL 3
#include "../common.h"

#include "../players/player_live_record.h"
#include "incoming_handler.h"

int32_t outgoing_handler(struct player_live_record *player);
