#pragma once

#include "../common.h"

#include "do_cmd_action.h"

#include "../mapping/coordinates.h"
#include "../mapping/room_blueprint.h"
#include "../mapping/room_atom.h"
#include "../mapping/calc_coords_from_playerloc_and_dir.h"
#include "../mapping/alterations.h"
#include "../mapping/lookup_room.h"
#include "../mapping/insert_room.h"
#include "../mapping/remove_room.h"

#include "../printing/printing.h"

#include "../players/player_details.h"
#include "../players/player_movement.h"

#include "../util/handling_incoming_players.h"

int32_t interpret_command(const size_t socket);
