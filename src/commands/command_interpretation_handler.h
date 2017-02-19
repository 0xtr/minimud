#pragma once

#include "../common.h"

#include "command_struct.h"
#include "command_registration_handler.h"
#include "../players/player_movement.h"
#include "../printing/print_not_player.h"
#include "../printing/print_to_player.h"
#include "../Map.h"
#include "../util/handling_incoming_players.h"
#include "../mapping/calc_coord_from_playerloc_and_dir.h"
#include "../mapping/room_adjustments.h"
#include "../mapping/lookup_room.h"
#include "../players/player_details.h"

int32_t interpret_command(const size_t pnum);
