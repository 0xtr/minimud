#pragma once

#include "../common.h"

#include "epollfd_storage.h"
#include "../players/PlayerLiveRecord.h"
#include "../mapping/room_adjustments.h"
#include "../commands/interpret_command.h"
#include "../util/outgoing_handler.h"
#include "../common.h"

int32_t incoming_handler(const int32_t socket);
int32_t shutdown_socket(const int32_t socket);
