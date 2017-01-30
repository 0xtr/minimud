Map *get_room () {
    Map *map = (Map*)calloc(sizeof(Map), sizeof(Map));
    return map;
}

void free_room (Map *map) {
    free(map);
}
