#pragma once

#include "../Players.h"
int32_t check_for_highest_socket_num (void);
int32_t check_if_player_is_already_online (const size_t pnum);
int32_t find_and_set_new_player_struct (const int32_t wfd);
int32_t ensure_player_moving_valid_dir (const int32_t socket_num, const uint8_t *command);
