#pragma once

#include "../util/check_player_name.h"
#include "../util/password_handling.h"
#include "../mapping/calc_coord_from_playerloc_and_dir.h"
#include "../mapping/remove_room.h"
#include "../mapping/insert_room.h"
#include "../mapping/adjust_room_details.h"
#include "../players/get_existing_player_hash.h"
#include "../players/lookup_player.h"
#include "../mapping/lookup_room_name_from_coords.h"
#include "../players/insert_player.h"
#include "../commands/command_parsing.h"
int32_t interpret_command (const int socket_num);
#include "interpret_command.c"
