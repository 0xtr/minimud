#pragma once

struct clist_l {
    uint8_t *cname;
    struct clist_l *next; 
};
typedef struct clist_l clist;

int32_t parse_clist (void);
int32_t check_clist (const int32_t socket_num, const uint8_t command[]);
int32_t line_begins_with_valid_chars (const uint8_t *line) __attribute__ ((unused));
#include <stdio.h>
#define _GNU_SOURCE
#include "../Commands.h"
#include "../printing/print_player_speech.h"
#include "../printing/print_output.h"
#include "../util/move_player.h"
#include "../util/shutdown_socket.h"
#include "../mapping/categorize_cmds.h"
#include "command_parsing.c"
