#include "incoming_handler.h"

int32_t check_if_data_is_waiting(const fd_set rfds)
{
	struct Player *tmp = get_player_head();
	while ((tmp = tmp->next) != NULL) {
		if (FD_ISSET(tmp->socket_num, &rfds))
			incoming_handler(tmp->socket_num);
	}
	return EXIT_SUCCESS;
}

int32_t incoming_handler(const int32_t socket)
{
	uint8_t *extra = NULL;
	int32_t retval;
	int32_t incoming_data_len = 0;

	ioctl (socket, FIONREAD, &incoming_data_len);

	if (incoming_data_len > BUFFER_LENGTH)
		extra = malloc(incoming_data_len - BUFFER_LENGTH); 

	retval = recv(socket, get_player_buffer(socket), sizeof(get_player_buffer(socket)), 0);

	if (retval == 0) {
		shutdown_socket(socket); 
		return EXIT_SUCCESS;
	} else if (retval == -1) {
		perror("Problem receiving data");
		return EXIT_FAILURE;
	}

	if (extra != NULL) {
		retval = recv(socket, extra, incoming_data_len - BUFFER_LENGTH, 0);
		memset(extra, '\0', incoming_data_len - BUFFER_LENGTH);
		free(extra);
		extra = NULL;
	}

	interpret_command(socket);

	return EXIT_SUCCESS;
}

int32_t shutdown_socket(const int32_t socket)
{
	if (shutdown(socket, SHUT_RDWR) == -1) {
		if (errno != ENOTCONN) {
			perror("Failed to shutdown a connection.\n");
			return EXIT_FAILURE;
		}
	}

	remove_player_by_socket(socket);
	return EXIT_SUCCESS;
}
