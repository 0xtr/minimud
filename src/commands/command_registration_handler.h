#pragma once

#include "../common.h"

#include "categorize_cmds.h"
#include "command_struct.h"
#include "../Players.h"
#include "../players/player_movement.h"
#include "../printing/print_not_player.h"
#include "../printing/print_to_player.h"

size_t get_num_of_available_cmds(void);
size_t get_total_length_of_all_cmds(void);
size_t get_max_command_len(void);
int32_t parse_clist(void);
int32_t check_clist(const int32_t socket, const uint8_t *command);
uint8_t *get_command(const int32_t cmd);
