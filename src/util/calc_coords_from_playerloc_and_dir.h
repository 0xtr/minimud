#pragma once

#include "../common.h"

#include "../rooms/coordinates.h"

#include "../players/player_details.h"

#include "../commands/classify_cmd.h"

struct coordinates calc_coords_from_playerloc_and_dir(struct player_live_record *player);
int32_t x_movement_to_vector(const struct command *info);
int32_t y_movement_to_vector(const struct command *info);
int32_t z_movement_to_vector(const struct command *info);
