#pragma once

#include "../common.h"
#include "../mapping/coordinates.h"

struct room_atom {
	int32_t id;

	_Bool found;

	uint8_t rname[NAMES_MAX];
	uint8_t rdesc[BUFFER_LENGTH];
	uint8_t owner[NAMES_MAX];
	uint8_t last_modified[NAMES_MAX];

	struct coordinates coords;

	int32_t exits[10];
}; 

struct room_atom *get_room(void);
