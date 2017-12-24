#pragma once

#include "../common.h"

#include "../players/player_live_record.h"

#include "../commands/classify_cmd.h"

#include "../rooms/room_actions.h"

#include "../util/outgoing_handler.h"

int32_t print_to_player(struct player_live_record *player, const int32_t argument);
int32_t greet_player(const int32_t socket);
int32_t print_player_speech(struct player_live_record *player);
int32_t print_room_to_player(struct player_live_record *player, struct room_db_record *room);
