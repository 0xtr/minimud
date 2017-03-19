#pragma once

#include "../common.h"

struct PlayerDBRecord {

	uint8_t *name;
	uint8_t *hash;
	uint8_t *salt;
	uint8_t *last_ip;

	int32_t x;
	int32_t y;
	int32_t z;

};

struct PlayerDBRecord *get_player_db_struct(void);
