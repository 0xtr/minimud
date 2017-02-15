#include "../common.h"
#include "movement_to_vector.h"

int32_t x_movement_to_vector (const uint8_t *cmd) {
    if (strcmp((char*)cmd, "east") == 0 ||
        strcmp((char*)cmd, "northeast") ||
        strcmp((char*)cmd, "southeast")) {
        return 1;
    } else if (strcmp((char*)cmd, "southwest") == 0 ||
               strcmp((char*)cmd, "northwest") ||
               strcmp((char*)cmd, "west")) {
        return -1;
    }
    return 0;
}

int32_t y_movement_to_vector (const uint8_t *cmd) {
    if (strcmp((char*)cmd, "north") == 0 ||
        strcmp((char*)cmd, "northeast") ||
        strcmp((char*)cmd, "northwest")) {
        return 1;
    } else if (strcmp((char*)cmd, "southeast") == 0 ||
               strcmp((char*)cmd, "southwest") ||
               strcmp((char*)cmd, "south")) {
        return -1;
    }
    return 0;
}

int32_t z_movement_to_vector (const uint8_t *cmd) {
    if (strcmp((char*)cmd, "up") == 0) {
        return 1;
    } else if (strcmp((char*)cmd, "down") == 0) {
        return -1;
    }
    return 0;
}
