#pragma once

#include "../common.h"

#include "../commands/classify_cmd.h"

int32_t x_movement_to_vector(const struct command *info);
int32_t y_movement_to_vector(const struct command *info);
int32_t z_movement_to_vector(const struct command *info);
