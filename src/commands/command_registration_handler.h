#pragma once

#include "command_struct.h"

size_t get_num_of_available_cmds(void);
size_t get_total_length_of_all_cmds(void);
size_t get_max_command_len(void);
int32_t parse_clist(void);
int32_t check_clist(const int32_t pnum, const uint8_t *command);
uint8_t *get_command(const int32_t cmd);

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include "../Players.h"
#include "../util/move_player.h"
#include "../printing/print_player_speech.h"

#include "command_registration_handler.c"
