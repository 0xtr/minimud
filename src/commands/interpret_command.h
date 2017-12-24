#pragma once

#include "../common.h"

#include "do_cmd_action.h"

#include "../rooms/room_blueprint.h"
#include "../rooms/room_db_record.h"

#include "../printing/printing.h"

#include "../players/player_details.h"
#include "../players/player_movement.h"

#include "../util/handling_incoming_players.h"
#include "../util/calc_coords_from_playerloc_and_dir.h"

int32_t interpret_command(struct player_live_record *player);
