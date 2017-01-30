static Player *player;
static Player *curr, *head, *root;

struct Player *get_player (const int32_t pnum) {
    return player[pnum];
}

struct Player *get_newest_player () {
    return curr;
}

uint8_t *get_player_store (const int32_t pnum) {
    return player[pnum]->store;
}

void set_player_hold_for_input (const int32_t pnum, const _Bool hold) {
    player[pnum]->hold_for_input = hold;
}

int32_t get_player_wait_state (const int32_t pnum) {
    return player[pnum]->wait_state;
}

void set_player_wait_state (const int32_t pnum, const int32_t wait_state) {
    player[pnum].wait_state = wait_state;
}

void set_player_pname (const int32_t pnum, const uint8_t *name) {
    strncpy((char*)player[pnum]->pname, (char*)name, NAMES_MAX);
}

void set_player_store_replace (const int32_t pnum, const uint8_t *newval) {
    strncpy((char*)player[pnum]->store, (char*)new, BUFFER_LENGTH);
}

void set_player_store_append (const int32_t pnum, const uint8_t *new) {
    strncat((char*)player[pnum]->store, (char*)new, BUFFER_LENGTH - strlen((char*)player[pnum]->store));
}

void clear_player_buffer (const int32_t pnum) {
    memset(player[pnum]->buffer, 0, BUFFER_LENGTH);
}

uint8_t *get_player_pname (const int32_t pnum) {
    return &player[pnum].pname;
}

void init_player_store (const int32_t pnum, const int32_t len) {
    player[pnum].store = calloc(NAMES_MAX, sizeof(uint8_t));
    player[pnum].store_size = len;
    strncpy((char*)player[pnum].store, (char*)player[pnum].buffer, BUFFER_LENGTH);
}

void clear_player_store (const int32_t pnum) {
    assert(player[pnum].store_size != 0);
    memset(player[pnum].store, '\0', player[pnum].store_size);
    free(player[pnum].store);
    player[pnum].store = NULL;
    player[pnum].store_size = 0;
}

_Bool get_player_hold_for_input (const int32_t pnum) {
    return player[pnum]->hold_for_input;
}

struct sockaddr *restrict get_newest_player_address () {
    return curr->address;
}

uint8_t *get_player_buffer (const int32_t pnum) {
    return player[pnum]->buffer;
}

_Bool get_player_in_use (const int32_t pnum) {
    return player[pnum]->in_use;
}

int32_t get_player_socket (const int32_t pnum) {
    return player[pnum].socket_num;
}

socklen_t *get_newest_player_address_len () {
    return curr->address_len;
}

void set_player_buffer_replace (const int32_t pnum, const uint8_t *newbuf) {
    strncpy((char*)player[pnum]->buffer, (char*)newbuf, BUFFER_LENGTH);
}

void set_player_buffer_append (const int32_t pnum, const uint8_t *append) {
    strncat((char*)player[pnum]->buffer, (char*)append, strlen((char*)player[pnum].buffer) - strlen((char*)append));
}

void remove_player_record (const int32_t pnum) {
    // remove prev and next and set surrounding records to each other
}

void remove_last_player_record () {
    printf("players taking %d bytes space\n", sizeof(player));
    free(head);
    head = curr->prev;
    head->next = NULL;
    head = curr;
    printf("players now taking %d bytes space\n", sizeof(player));
}

struct Player* get_new_player () {
    printf("players taking %d bytes space\n", sizeof(player));
    curr = (Player*)malloc(sizeof(Player));
    curr->prev = head;
    head->next = curr;
    head = curr;
    printf("players now taking %d bytes space\n", sizeof(player));
    return head;
}

size_t get_num_of_players () {
    size_t list_size = 0;
    Player *tmp = head;
    for (list_size = 0; tmp->prev != NULL; ++list_size) {
    }
    return list_size;
}

int32_t getplayernum (const int32_t socknum) {
    int32_t i;
    _Bool found = false;
    Player *tmp = head;
    for (i = 0; tmp->prev != NULL; ++i) {
        if (player[i].socknum == socknum) {
            found = true;
            break;
        }
    }
    if (found == 1) {
        return i;
    } else {
        return -1; 
    }
}
