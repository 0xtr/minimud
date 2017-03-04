#pragma once

#define CHARS_FOR_PROMPT_AND_NULL 2
#include "../common.h"

#include "../Players.h"
#include "incoming_handler.h"

int32_t outgoing_handler(const int32_t socket);
