#include "classify_cmd.h"

const char * const valid_directions[] = {
	"north", "n",
	"east", "e",
	"south","s",
	"west","w",
	"up","u",
	"down","d",
	"northwest","nw", 
	"northeast", "ne", 
	"southeast", "se", 
	"southwest", "sw"
};

int32_t is_direction (const uint8_t *cmd) {
	const size_t len = strlen((char *)cmd);

	for (size_t i = 0; i < 20; ++i) {
		if (memcmp(valid_directions[i], cmd, len) == 0)
			return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}
