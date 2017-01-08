#pragma once

#include <stdio.h>
#define _GNU_SOURCE
#include "../Players.h"
#include "../Commands.h"
#include "../printing/print_player_speech.h"
#include "../printing/print_output.h"
#include "../util/move_player.h"
#include "../util/shutdown_socket.h"
#include "../mapping/categorize_cmds.h"

struct clist_l {
    uint8_t *cname;
    struct clist_l *next; 
};
typedef struct clist_l clist;

int32_t parse_clist (void);
int32_t check_clist (const int32_t pnum, const uint8_t *command);
int32_t line_begins_with_valid_chars (const uint8_t *line) __attribute__ ((unused));
size_t get_total_length_of_all_cmds ();
#include "command_parsing.c"
