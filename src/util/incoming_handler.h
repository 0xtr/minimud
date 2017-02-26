#pragma once

#include "../common.h"

#include "../mapping/room_adjustments.h"
#include "../commands/command_interpretation_handler.h"
#include "../util/outgoing_handler.h"
#include "../util/incoming_handler.h"
#include "../Players.h"
#include "../common.h"

int32_t incoming_handler(const int32_t socket_num); 
int32_t shutdown_socket(const int32_t playernum);
int32_t check_if_data_is_waiting(const fd_set rfds);
