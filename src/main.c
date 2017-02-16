#include "common.h"
#include "sqlite/init_db.h"
#include "commands/command_registration_handler.h"
#include "util/check_if_data_is_waiting.h"
#include "printing/write_greeting.h"
#include "util/handling_incoming_players.h"

int32_t main(void)
{
	int32_t new_fd, fdmax;
	struct sockaddr_in address;
	int32_t master_sockfd;
	fd_set rfds;

	// open the sqlite3 db connections for rooms & players 
	assert(init_db(ROOM_DB_TYPE) == EXIT_SUCCESS);
	assert(init_db(PLAYER_DB_TYPE) == EXIT_SUCCESS);
	// build the list of commands from text file into memory
	assert(parse_clist() == EXIT_SUCCESS);

	// create the master socket 
	master_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (master_sockfd <= 0) {
		perror("Could not create a socket");
		return EXIT_FAILURE;
	}

	// bind it to a port
	address.sin_family	= AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port	= htons(5000);
	if (bind(master_sockfd, (struct sockaddr *)&address, sizeof(address)) != 0) {
		perror("Problem binding socket to a port");
		return EXIT_FAILURE;
	}
	
	// set listener for connections
	if (listen(master_sockfd, SOMAXCONN) != 0) {
		perror("Listening for connections failed");
		return EXIT_FAILURE;
	}
	fprintf(stdout, "%s 5000 on socket %d\n", "WAIT: Now listening for connections", master_sockfd);

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
			//Player *player = get_new_player();
			new_fd = accept(master_sockfd, get_newest_player_address(), get_newest_player_address_len());
			if (new_fd == -1) {
				perror("Failed to accept incoming connection");
				break;
			}
			if ((find_and_set_new_player_struct(new_fd)) == 0) {
				perror("Error setting up the new player's connection");
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
