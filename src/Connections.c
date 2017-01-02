static int32_t num_of_connections;

int32_t set_active_conns (const int32_t value) {
    return (num_of_connections = value);
}

int32_t get_active_conns () {
    return num_of_connections;
}

void decrement_active_conns () {
    --num_of_connections;
}

void increment_active_conns () {
    ++num_of_connections;
}
