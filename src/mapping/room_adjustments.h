#pragma once

#include "../common.h"

#include "NewRoom.h"
#include "../players/player_movement.h"
#include "../sqlite/helper_funcs.h"
#include "lookup_room.h"

int32_t adjust_room_details(const int32_t adjusting, const _Bool reverse, const int32_t pnum, const int32_t x, const int32_t y, const int32_t z);
int32_t get_opposite_dir(const uint8_t *dir);
int32_t remove_players_from_room(const int32_t x, const int32_t y, const int32_t z);
int32_t insert_room(struct NewRoom rconfig);
int32_t remove_room(const int32_t pnum);
uint8_t *get_dir_string(const int32_t dir);
int32_t get_direction_as_number(const uint8_t *dir);
