#pragma once

#include "command_struct.h"
#include "../printing/print_player_speech.h"
#include "../printing/print_output.h"
#include "../util/move_player.h"
#include "../util/shutdown_socket.h"
#include "../mapping/categorize_cmds.h"
int32_t parse_clist (void);
#include "command_parsing.c"
int32_t check_clist (const int32_t socknum, const uint8_t command[]);
