#include "incoming_handler.h"

int32_t check_if_data_is_waiting(const fd_set rfds)
{
	for (size_t k = 0; k <= get_num_of_players(); ++k) {
		if (FD_ISSET(get_player_socket(k), &rfds)) {
			incoming_handler(get_player_socket(k));
		}
	}
	return EXIT_SUCCESS;
}

int32_t incoming_handler(const int32_t socket_num)
{
	uint8_t *extra;
	int32_t retval;
	int32_t pnum = getplayernum(socket_num);
	int32_t incoming_data_len = 0;

	ioctl (socket_num, FIONREAD, &incoming_data_len);

	if (incoming_data_len > BUFFER_LENGTH)
		extra = malloc(incoming_data_len - BUFFER_LENGTH); 

	retval = recv(socket_num, get_player_buffer(pnum), sizeof(get_player_buffer(pnum)), 0);

	if (retval == 0) {
		shutdown_socket(pnum); 
		return EXIT_SUCCESS;
	} else if (retval == -1) {
		perror("Problem receiving data.");
		return EXIT_FAILURE;
	}

	if (extra != NULL) {
		retval = recv(socket_num, extra, incoming_data_len - BUFFER_LENGTH, 0);
		memset(extra, '\0', incoming_data_len - BUFFER_LENGTH);
		free(extra);
		extra = NULL;
	}

	interpret_command(pnum);

	return EXIT_SUCCESS;
}

int32_t shutdown_socket(const int32_t pnum)
{
	if (shutdown(get_player_socket(pnum), SHUT_RDWR) == -1) {
		if (errno != ENOTCONN) {
			perror("Failed to shutdown a connection.\n");
			return EXIT_FAILURE;
		}
	}

	remove_player_record(pnum);
	return EXIT_SUCCESS;
}
