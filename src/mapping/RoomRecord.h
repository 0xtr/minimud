#pragma once

#include "../common.h"

struct RoomRecord {
	int32_t id;
	uint8_t rname[NAMES_MAX];
	uint8_t rdesc[BUFFER_LENGTH];
	uint8_t owner[NAMES_MAX];
	uint8_t last_modified[NAMES_MAX];
	_Bool north;
	_Bool south;
	_Bool west;
	_Bool east;
	_Bool down;
	_Bool up;
	_Bool northeast;
	_Bool southeast;
	_Bool southwest;
	_Bool northwest;
}; 
struct RoomRecord *get_room(void);
void free_room(struct RoomRecord *room);
