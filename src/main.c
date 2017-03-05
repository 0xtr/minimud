#include "common.h"
#include "sqlite/init_db.h"
#include "commands/command_registration_handler.h"
#include "util/incoming_handler.h"
#include "printing/print_to_player.h"
#include "util/handling_incoming_players.h"

static int32_t set_socket_nonblocking(const int32_t socket);

int32_t main(void)
{
	// open the sqlite3 db connections for rooms & players 
	assert(init_dbs() == EXIT_SUCCESS);
	// build the list of commands from text file into memory
	assert(parse_clist() == EXIT_SUCCESS);

	// create the master socket 
	const int32_t listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket <= 0) {
		perror("Could not create a socket");
		return EXIT_FAILURE;
	}
	set_socket_nonblocking(listen_socket);

	// bind it to a port
	struct sockaddr_in address;
	address.sin_family	= AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port	= htons(5000);
	if (bind(listen_socket, (struct sockaddr *)&address, sizeof(address)) != 0) {
		perror("Problem binding socket to a port");
		return EXIT_FAILURE;
	}
	
	// set listener for connections
	if (listen(listen_socket, SOMAXCONN) != 0) {
		perror("Listening for connections failed");
		return EXIT_FAILURE;
	}

	const int32_t epollfd = epoll_create1(0);
	if (epollfd == -1) {
		perror("Epoll instance creation failed");
		return EXIT_FAILURE;
	}

	int32_t new_fd, nfds;
	struct epoll_event ev, events[MAX_SIMULTANEOUS_EPOLL_EVENTS];
	ev.events = EPOLLIN;
	ev.data.fd = listen_socket;

	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_socket, &ev) == -1) {
		perror("Epoll ctl ops for epollfd failed");
		return EXIT_FAILURE;
	}

	for (;;) {
		nfds = epoll_wait(epollfd, events, MAX_SIMULTANEOUS_EPOLL_EVENTS, -1);
		if (nfds == -1) {
			perror("Epoll wait failed");
			return EXIT_FAILURE;
		}

		for (int32_t i = 0; i < nfds; ++i) {
			if (events[i].data.fd == listen_socket) {
				new_fd = accept(listen_socket, NULL, NULL);
				if (new_fd == -1) {
					perror("Failed to accept incoming connection");
					return EXIT_FAILURE;
				}

				set_socket_nonblocking(new_fd);
				ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
				ev.data.fd = new_fd;
				if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_fd, &ev) == -1) {
					perror("Epoll ctl ops failed");
					return EXIT_FAILURE;
				}

				if ((add_new_player(new_fd)) == EXIT_FAILURE) {
					perror("Error setting up the new player's connection");
					return EXIT_FAILURE;
				}

				greet_player(new_fd);
			} else {
				incoming_handler(events[i].data.fd);
			}
		}
	}

	perror("minimud server exited");
	return EXIT_FAILURE;
}

static int32_t set_socket_nonblocking(const int32_t socket)
{
	if (fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
		perror("Fcntl failed: set socket nonblocking");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
