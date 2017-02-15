#pragma once

#include "command_struct.h"
#include "../printing/echoaround_player.h"
#include "../printing/print_output.h"
#include "../Map.h"
#include "../util/check_player_name.h"
#include "../util/password_handling.h"
#include "../mapping/calc_coord_from_playerloc_and_dir.h"
#include "../mapping/room_adjustments.h"
#include "../mapping/lookup_room.h"
#include "../players/player_details.h"

int32_t interpret_command(const size_t pnum);
