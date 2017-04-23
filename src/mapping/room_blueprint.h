#pragma once

struct room_blueprint {
	uint8_t *name;
	uint8_t *desc;

	struct coordinates coords;

	uint8_t *owner;
	uint8_t *flags;
};
