#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include "../Players.h"
#include "../printing/print_player_speech.h"
#include "../Commands.h"

struct clist_l {
    uint8_t *cname;
    struct clist_l *next; 
};
typedef struct clist_l clist;

int32_t parse_clist (void);
int32_t check_clist (const int32_t pnum, const uint8_t *command);
size_t get_total_length_of_all_cmds();
size_t set_num_of_available_cmds ();
uint8_t *get_command (const int32_t cmd);
#include "command_parsing.c"
