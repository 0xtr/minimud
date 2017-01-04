struct Player_l {
    _Bool   in_use;
    _Bool   hold_for_input;
    int32_t wait_state;
    int32_t socket_num;
    uint8_t pname[NAMES_MAX];
    uint8_t holder[NAMES_MAX];
    struct  sockaddr *restrict address;
    socklen_t address_len;
    uint8_t buffer[BUFFER_LENGTH];
    void    *store;
    int32_t store_size;
    struct  Player* prev;
    struct  Player* next;
};
typedef struct Player_l Player;
static Player *player;
static Player *curr, *head, *root;

struct Player* get_player (const int32_t pnum) {
    return player[pnum];
}

struct Player *get_newest_player () {
    return curr;
}

int32_t get_player_wait_state (const int32_t pnum) {
    return player[pnum]->wait_state;
}

void set_player_wait_state (const int32_t pnum, const int32_t wait_state) {
    player[pnum].wait_state = wait_state;
}

void set_player_pname (const int32_t pnum, const uint8_t *name) {
    strncpy((char*)player[pnum].pname, (char*)name, NAMES_MAX);
}

uint8_t *get_player_pname (const int32_t pnum) {
    return &player[pnum].pname;
}

void init_player_store (const int32_t pnum) {
    player[pnum].store = calloc(NAMES_MAX, sizeof(uint8_t));
    player[pnum].store_size = NAMES_MAX;
    strncpy((char*)player[pnum].store, (char*)player[pnum].buffer, NAMES_MAX);
    set_player_wait_state (pnum, WAIT_CONFIRM_NEW_ROOM_NAME);
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

int32_t get_num_of_players () {
    int32_t list_size = 0;
    Player *tmp = head;
    for (list_size = 0; tmp->prev != NULL; ++list_size) {
    }
    return list_size;
}

int32_t getplayernum (const int32_t socknum) {
    int32_t i;
    _Bool found = 0;
    Player *tmp = head;
    for (i = 0; tmp->prev != NULL; ++i) {
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
