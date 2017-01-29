#define _GNU_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <assert.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stropts.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <sqlite3.h>
#include <stdint.h>
#include <sys/select.h>

#include "src/Players.h"
#include "src/DEFINITIONS.h"
#include "src/util/handling_incoming_players.h"
#include "src/printing/write_greeting.h"
#include "src/util/check_if_data_is_waiting.h"
#include "src/commands/command_parsing.h"
#include "src/sqlite/init_db.h"

int32_t main (void) {
    int32_t new_fd, fdmax;
    // open the sqlite3 db connections for rooms & players 
    assert(init_db(ROOM_DB_TYPE) == EXIT_SUCCESS);
    assert(init_db(PLAYER_DB_TYPE) == EXIT_SUCCESS);
    // build the list of commands from text file into memory
    assert(parse_clist() == EXIT_SUCCESS);

    // create the master socket 
    const int32_t master_sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    if (listen(master_sockfd, SOMAXCONN) != 0) {
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
        // largest file descriptor tracking
        fdmax = master_sockfd;

        // get max socket file descriptor
        fdmax = check_for_highest_socket_num();
        // select, should be replaced with poll?
        assert(select(fdmax + 1, &rfds, NULL, NULL, NULL) != -1);
        // struct pollfd fds[], nfds_t nfds, int32_t timeout args to poll
        // status = poll(

        if (FD_ISSET(master_sockfd, &rfds)) {
            get_new_player();
            new_fd = accept(master_sockfd, get_newest_player_address(), get_newest_player_address_len());
            if (new_fd == -1) {
                log_issue((u_int8_t*)"Failed to accept incoming connection.");
                break;
            }
            if ((find_and_set_new_player_struct(new_fd)) == 0) {
                log_issue((u_int8_t*)"Error setting up the new player's connection!");
                return EXIT_FAILURE;
            }
            write_greeting(new_fd);
        }
        
        // don't need to account for EINTR/EAGAIN, no signals + nonblocking io
        check_if_data_is_waiting(rfds);
    }

    fprintf(stdout, "%s\n", "minimud server exited. Check error log.");
    return EXIT_FAILURE;
}
