int32_t get_opposite_dir (uint8_t dir[]) {
    int32_t i, found = 0;
    for (i = 0; i != VALID_DIRS; ++i) {
        if (strcmp((char*)valid_dirs[i], (char*)dir) == 0) {
            found = 1;
            break;
        }
    }
    if (found == 0) {
        return -1;
    }
    if (strcmp(dir, "south") == 0     ||
        strcmp(dir, "east") == 0      ||
        strcmp(dir, "southwest") == 0 ||
        strcmp(dir, "southeast") == 0 ||
        strcmp(dir, "down") == 0) {
        return i - 1;
    } else {
        return i + 1;
    }
    return -1;
}
