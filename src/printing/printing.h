#pragma once

#include "../common.h"

#include "../players/PlayerLiveRecord.h"

#include "../commands/classify_cmd.h"

#include "../mapping/coordinates.h"
#include "../mapping/lookup_room.h"

#include "../util/outgoing_handler.h"

int32_t print_to_player(const int32_t socket, const int32_t argument);
int32_t greet_player(const int32_t socket);
int32_t print_player_speech(const int32_t socket);
