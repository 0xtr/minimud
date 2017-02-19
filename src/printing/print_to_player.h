#pragma once

#include "../common.h"

#include "../Players.h"
#include "../mapping/lookup_room.h"
#include "../util/outgoing_handler.h"

int32_t print_to_player(const int32_t pnum, const int32_t argument);
int32_t greet_player(const int32_t player_num);
int32_t print_player_speech_to_player(const int32_t pnum, const uint8_t *say);
