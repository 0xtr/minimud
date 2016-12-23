#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h> // assert or assert_perror
#include <netinet/in.h>
#include <netdb.h>
#include <stropts.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sqlite3.h>
#include <stdint.h>
#include <sys/select.h>

#include "src/ERR_AND_STATE_CODES.h"
#include "src/util/handling_incoming_players.h"
#include "src/printing/write_greeting.h"
#include "src/util/check_if_data_is_waiting.h"
#include "src/commands/command_parsing.h"
#include "src/sqlite/init_db.h"

/* parameters below */
#define  IP_LEN           20
#define  BUFFER_LENGTH    512
#define  NAMES_MIN        5
#define  NAMES_MAX        45

/* map config */
#define  MAP_WIDTH        0
#define  MAP_HEIGHT       0
#define  MAP_DEPTH        0
#define  START_LOC_X      0
#define  START_LOC_Y      0
#define  START_LOC_Z      0
#define  PRINT_LINE_WIDTH 56

// globals
size_t new_fd, fdmax;
int32_t conns = 1;

typedef struct players {
    int     in_use:1;
    int     hold_for_input:1;
    // unused 2 padding
    int32_t wait_state;
    int32_t socket_num;
    uint8_t pname[NAMES_MAX];
    uint8_t holder[NAMES_MAX];
    struct  sockaddr *restrict address;
    socklen_t address_len;
    uint8_t buffer[BUFFER_LENGTH];
    void    *store;
    int32_t store_size;
} players;
players *player;

int main (void) {
    int32_t status = 0;

    // open the sqlite3 db connections for rooms & players 
    assert(init_db(ROOM_DB_TYPE) == 1);
    assert(init_db(PLAYER_DB_TYPE) == 1);
    // build the list of commands from text file into memory
    assert(parse_clist() == 1);
    // declare and clean out all the player structs

    // create the master socket 
    int32_t master_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (master_sockfd <= 0) {
        log_issue((uint8_t*)"Could not create a socket. Exiting.");
        return EXIT_FAILURE;
    }

    // bind it to a port
    struct sockaddr_in address;
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(5000);
    if (bind(master_sockfd, (struct sockaddr *)&address, sizeof(address)) != 0) {
        log_issue((u_int8_t*)"Problem binding socket to a port.");
        return EXIT_FAILURE;
    }
    
    // set listener for connections
    if ((status = listen(master_sockfd, SOMAXCONN)) != 0) {
        log_issue((u_int8_t*)"Listening for connections failed.");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "%s 5000 on socket %d\n", "WAIT: Now listening for connections", master_sockfd);

    fd_set rfds;
    for (;;) {
        // zero rfds
        FD_ZERO (&rfds);
        // add the listening socket
        FD_SET (master_sockfd, &rfds);
        // largest fds tracking
        fdmax = master_sockfd;

        // get max socket file descriptor
        for (size_t j = 0; j < get_active_conns(); ++j) {
            if (player[j].socket_num > 0) {
                FD_SET(player[j].socket_num, &rfds);
                if (player[j].socket_num > fdmax) {
                    fdmax = player[j].socket_num;
                }
            }
        }
        // select, should be replaced with poll?
        assert(select(fdmax + 1, &rfds, NULL, NULL, NULL) != -1);
        // struct pollfd fds[], nfds_t nfds, int32_t timeout args to poll
        // status = poll(

        if (FD_ISSET(master_sockfd, &rfds)) {
            new_fd = accept(master_sockfd, player[conns].address, &player[conns].address_len);
            if (new_fd == -1) {
                log_issue((u_int8_t*)"Failed to accept incoming connection.");
                break;
            }
            if ((find_and_set_new_player_struct(new_fd)) == 0) {
                fprintf(stdout, "%s\n", "Error setting up the new player's connection!");
                return EXIT_FAILURE;
            }
            write_greeting(new_fd);
        }
        
        // don't need to account for EINTR/EAGAIN, no signals + nonblocking io
        check_if_data_is_waiting(fdmax, rfds);
    }

    fprintf(stdout, "%s\n", "minimud server exited. Check error log.");
    return 0;
}
