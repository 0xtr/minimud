#pragma once

#include "../common.h"

#include "../util/outgoing_handler.h"
int32_t print_not_player(const int32_t socket, const uint8_t *buffer, const int32_t location);
int32_t print_player_speech_to_others(const int32_t socket, const uint8_t *say);
