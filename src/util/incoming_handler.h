#pragma once

#include "../common.h"

#include "epollfd_storage.h"
#include "../players/player_live_record.h"
#include "../rooms/room_actions.h"
#include "../commands/interpret_command.h"
#include "../util/outgoing_handler.h"
#include "../common.h"

int32_t incoming_handler(const int32_t socket);
int32_t shutdown_socket(struct player_live_record *player);
