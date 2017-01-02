#pragma once

int32_t set_active_conns (const int32_t value);
int32_t get_active_conns ();
void decrement_active_conns ();
void increment_active_conns ();
#include "Connections.c"
