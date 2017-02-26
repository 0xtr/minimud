#pragma once

struct NewRoom {
	uint8_t *name;
	uint8_t *desc;

	int32_t x;
	int32_t y;
	int32_t z;

	uint8_t *owner;
	uint8_t *flags;
};
