#include "incoming_handler.h"

static void strip_carriage_returns(const int32_t socket);

int32_t incoming_handler(const int32_t socket)
{
	int32_t retval;
	int32_t incoming_data_len = 0;

	ioctl (socket, FIONREAD, &incoming_data_len);

	uint8_t *buffer = calloc(incoming_data_len+1, sizeof(uint8_t));

	retval = recv(socket, buffer, incoming_data_len, 0);

	if (incoming_data_len > BUFFER_LENGTH)
		memset(&buffer[BUFFER_LENGTH], 0, incoming_data_len - BUFFER_LENGTH);

	clear_player_buffer(socket);
	set_player_buffer_replace(socket, buffer);

	free(buffer);

	if (retval == 0) {
		return shutdown_socket(socket); 
	} else if (retval == -1) {
		if (errno == EAGAIN)
			return EXIT_SUCCESS;

		perror("Problem receiving data");
		return EXIT_FAILURE;
	}

	strip_carriage_returns(socket);
	interpret_command(socket);

	return EXIT_SUCCESS;
}

static void strip_carriage_returns(const int32_t socket)
{
	uint8_t *buffer = get_player_buffer(socket);

	for (size_t i = 0; i < strlen((char *)buffer); ++i) {
		if (buffer[i] == '\r')
			buffer[i] = '\0';
	}
}

int32_t shutdown_socket(const int32_t socket)
{
	if (shutdown(socket, SHUT_RDWR) == -1) {
		if (errno != ENOTCONN) {
			perror("Failed to shutdown a connection");
			return EXIT_FAILURE;
		}
	}

	if (epoll_ctl(get_epollfd(), EPOLL_CTL_DEL, socket, NULL) == -1)
		perror("Failed to remove socket from epoll list");

	remove_player_by_socket(socket);
	return EXIT_SUCCESS;
}
