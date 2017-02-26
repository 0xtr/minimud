#include "categorize_cmds.h"

int32_t is_direction (const uint8_t *cmd) {
	const uint8_t dirs[20][10] = {
		"north","n","east","e","south","s","west","w",
		"up","u","down","d", "northwest","nw", "northeast",
		"ne", "southeast", "se", "southwest", "sw"
	};

	for (size_t i = 0; i < 20; ++i) {
		if (strcmp((char*)dirs[i], (char*)cmd) == 0)
			return EXIT_SUCCESS;
	}

	if (strcmp((char *)cmd, (char *)"return") || 
	    strcmp((char *)cmd, (char *)"Return"))
		return EXIT_SUCCESS;

	return EXIT_FAILURE;
}
