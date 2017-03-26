#pragma once

#include "../common.h"

struct Command {
	uint8_t *cname;
	struct Command *next; 
	struct Command *prev; 
};

#include "classify_cmd.h"
#include "../players/PlayerLiveRecord.h"
#include "../players/player_movement.h"
#include "../printing/print_not_player.h"
#include "../printing/print_to_player.h"

size_t get_num_of_available_cmds(void);
size_t get_total_length_of_all_cmds(void);
size_t get_max_command_len(void);
int32_t parse_clist(void);
int32_t check_cmd_against_list(const uint8_t *command);
int32_t execute_player_command(const int32_t socket, const uint8_t *command);
uint8_t *get_command(const int32_t cmd);
