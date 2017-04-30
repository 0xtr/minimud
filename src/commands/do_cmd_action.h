#pragma once

#include "../common.h"

#include "classify_cmd.h"

#include "../players/player_live_record.h"
#include "../players/player_movement.h"

#include "../printing/printing.h"

int32_t do_cmd_action(const int32_t socket, struct command *info);
