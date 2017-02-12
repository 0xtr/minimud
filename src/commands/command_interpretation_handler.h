#pragma once

#include "command_struct.h"

int32_t interpret_command(const size_t pnum);

#include "../Map.h"
#include "../util/check_player_name.h"
#include "../util/password_handling.h"
#include "../mapping/calc_coord_from_playerloc_and_dir.h"
#include "../mapping/remove_room.h"
#include "../mapping/insert_room.h"
#include "../mapping/adjust_room_details.h"
#include "../players/get_player_details.h"
#include "../players/lookup_player.h"
#include "../mapping/lookup_room.h"
#include "../players/insert_player.h"

#include "command_interpretation_handler.c"
