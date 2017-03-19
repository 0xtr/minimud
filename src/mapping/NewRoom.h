#pragma once

struct NewRoom {
	uint8_t *name;
	uint8_t *desc;

	struct Coordinates coords;

	uint8_t *owner;
	uint8_t *flags;
};
