#pragma once
    
#include "../common.h"

struct Inventory {
	int32_t count;
};

struct Inventory *get_new_player_inventory(const int32_t socket);
