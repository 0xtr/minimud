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

struct sockaddr *restrict get_newest_player_address () {
    return curr->address;
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
