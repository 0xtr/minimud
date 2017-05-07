#include "incoming_handler.h"

static void strip_carriage_returns(struct player_live_record *player);

int32_t incoming_handler(const int32_t socket)
{
	int32_t retval;
	int32_t incoming_data_len = 0;

	ioctl (socket, FIONREAD, &incoming_data_len);

	uint8_t *buffer = calloc(incoming_data_len+1, sizeof(uint8_t));

	retval = recv(socket, buffer, incoming_data_len, 0);

	if (incoming_data_len > BUFFER_LENGTH)
		memset(&buffer[BUFFER_LENGTH], 0, incoming_data_len - BUFFER_LENGTH);

	struct player_live_record *player = get_player(socket);
	clear_player_buffer(player);
	set_player_buffer_replace(player, buffer);

	free(buffer);

	if (retval == 0) {
		return shutdown_socket(player); 
	} else if (retval == -1) {
		if (errno == EAGAIN)
			return EXIT_SUCCESS;

		perror("Problem receiving data");
		return EXIT_FAILURE;
	}

	strip_carriage_returns(player);
	interpret_command(player);

	return EXIT_SUCCESS;
}

static void strip_carriage_returns(struct player_live_record *player)
{
	for (size_t i = 0; i < strlen((char *)player->buffer); ++i) {
		if (player->buffer[i] == '\r')
			player->buffer[i] = '\0';
	}
}

int32_t shutdown_socket(struct player_live_record *player)
{
	if (shutdown(player->socket_num, SHUT_RDWR) == -1) {
		if (errno != ENOTCONN) {
			perror("Failed to shutdown a connection");
			return EXIT_FAILURE;
		}
	}

	if (epoll_ctl(get_epollfd(), EPOLL_CTL_DEL, player->socket_num, NULL) == -1)
		perror("Failed to remove socket from epoll list");

	remove_player_by_socket(player->socket_num);

	return EXIT_SUCCESS;
}
