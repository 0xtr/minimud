int32_t getplayernum (final int32_t socknum) {
    int32_t i;
    int32_t found = 0;
    for (i = 0; i < get_active_conns(); ++i) {
        if (player[i].socknum == socknum) {
            found = 1;
            break;
        }
    }
    if (found == 1) {
        return i;
    } else {
        return -1; 
    }
}
