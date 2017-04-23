#pragma once

#include "../common.h"

struct PlayerDBRecord {

	uint8_t name[NAMES_MAX];
	uint8_t hash[HASH_LENGTH];
	uint8_t salt[SALT_LENGTH];
	uint8_t last_ip[8]; // sort out

	int32_t id;
	int32_t x;
	int32_t y;
	int32_t z;

};

struct PlayerDBRecord *get_player_db_struct(void);
