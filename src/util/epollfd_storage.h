#pragma once

#include "../common.h"

int32_t get_epollfd(void);
void set_epollfd(const int32_t fd);
