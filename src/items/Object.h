#pragma once

#include "../common.h"

struct Object {
	int32_t id;
	uint8_t *obj_name, *obj_desc, *obj_keywords[];
	int32_t obj_weight;
};
