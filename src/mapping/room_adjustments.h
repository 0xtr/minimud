#pragma once

#include "../common.h"

#include "../mapping/Coordinates.h"
#include "../mapping/RoomRecord.h"
#include "../mapping/lookup_room.h"
#include "../mapping/NewRoom.h"

#include "../players/player_movement.h"

#include "../sqlite/helper_funcs.h"

int32_t adjust_room_details(const int32_t adjusting, const int32_t socket, const struct Coordinates coords);
int32_t get_opposite_dir(const uint8_t *dir);
int32_t remove_players_from_room(const struct Coordinates coords);
uint8_t *get_dir_string(const int32_t dir);
int32_t get_direction_as_number(const uint8_t *dir);
int32_t adjust_room_exit(const int32_t socket, const struct Coordinates player_coords, const struct Coordinates new_coords);
