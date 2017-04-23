#pragma once

#include "../common.h"

#include "../mapping/coordinates.h"

#include "../players/player_details.h"

#include "../commands/classify_cmd.h"

#include "movement_to_vector.h"

struct coordinates calc_coords_from_playerloc_and_dir(const int32_t socket);
