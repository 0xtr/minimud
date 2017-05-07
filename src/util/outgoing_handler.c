#include "outgoing_handler.h"

static float get_buffer_split_by_line_width(const int32_t expected);
static int32_t add_prompt_chars(struct player_live_record *player);
static _Bool all_data_was_sent(const int32_t total, const int32_t expected);
static size_t find_reasonable_line_end(struct player_live_record *player, const int32_t buffer_pos);
static int32_t send_and_handle_errors(struct player_live_record *player, const int32_t expected);
static size_t num_of_newlines(struct player_live_record *player);

int32_t outgoing_handler(struct player_live_record *player)
{
	const size_t buflen = strlen((char *)player->buffer);
	size_t expected = (buflen > (BUFFER_LENGTH - 1)) ? BUFFER_LENGTH : buflen;
	size_t buffer_pos = 0;

	if ((1 + strlen((char *)player->buffer)) <= PRINT_LINE_WIDTH) {
		expected += add_prompt_chars(player);
		return send_and_handle_errors(player, expected);
	}

	const double LINES_REQUIRED_FOR_MSG = 
		get_buffer_split_by_line_width(strlen((char *)player->buffer))
		+ num_of_newlines(player);

	uint8_t *processed_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));
	void *loc_in_buf = mempcpy(processed_buf, "", 0);

	for (size_t iters = 0; iters < LINES_REQUIRED_FOR_MSG; ++iters) {
		const size_t stop_at_char = find_reasonable_line_end(player, buffer_pos);

		loc_in_buf = mempcpy(loc_in_buf, &player->buffer[buffer_pos], stop_at_char);
		loc_in_buf = mempcpy(loc_in_buf, "\n", 1);

		buffer_pos += stop_at_char + 1;
		if (buffer_pos > expected)
			break;
	}

	loc_in_buf = mempcpy(loc_in_buf, "\n", 1);
	set_player_buffer_replace(player, processed_buf);
	free(processed_buf);

	return send_and_handle_errors(player, expected);
}

static size_t num_of_newlines(struct player_live_record *player)
{
	size_t newlines = 0;
	for (size_t i = 0; i < strlen((char *)player->buffer); ++i) {
		if (player->buffer[i] == '\n')
			++newlines;
	}
	return newlines;
}

static size_t find_reasonable_line_end(struct player_live_record *player, const int32_t buffer_pos)
{
	int32_t last_space = 0;
	uint8_t *last_match, *substr = calloc(PRINT_LINE_WIDTH+1, sizeof(uint8_t));

	memcpy(substr, &player->buffer[buffer_pos], PRINT_LINE_WIDTH);
	const size_t substr_len = strlen((char *)substr);

	if ((last_match = (uint8_t *)strrchr((char *)substr, ' ')) != NULL)
		last_space = last_match - substr;

	if ((last_match = (uint8_t *)strchr((char *)substr, '\n')) != NULL) {
		last_space = last_match - substr;
		free(substr);
		return last_space;
	}

	free(substr);

	if (substr_len < PRINT_LINE_WIDTH)
		return substr_len;

	if (((last_space/(double)PRINT_LINE_WIDTH)*100) < 70)
		return PRINT_LINE_WIDTH;

	return last_space;
}

static float get_buffer_split_by_line_width(const int32_t expected)
{
	float integral;
	float lines = expected / PRINT_LINE_WIDTH;

	modff (lines, &integral);
	lines -= integral;

	if (lines != 0.0)
		++integral;

	return integral;
}

static int32_t send_and_handle_errors(struct player_live_record *player, const int32_t expected)
{
	#define MAX_ATTEMPTS 10
	int32_t returned, total = 0;

	for (size_t i = 0; i < MAX_ATTEMPTS; ++i) {
		returned = send(player->socket_num, &player->buffer[total], expected, 0);
		if (returned != -1) {
			total += returned;
			if (all_data_was_sent(total, expected) == true) {
				break;
			} else {
				continue;
			}
		}
		switch (errno) {
			case ENOTCONN:
			case ECONNRESET:
				perror("Socket needs to be terminated; client left");
				shutdown_socket(player);
				break;
			case EMSGSIZE:
				perror("Tried to send a message that was too large");
				exit(EXIT_FAILURE);
			default:
				perror("Failure sending data");
				continue;
		}
	}

	clear_player_buffer(player);
	return EXIT_SUCCESS;
}

static _Bool all_data_was_sent(const int32_t total, const int32_t expected)
{
	return total == expected;
}

static int32_t add_prompt_chars(struct player_live_record *player)
{
	uint8_t *tmp_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));
	void *loc_in_buf;

	loc_in_buf = mempcpy(tmp_buf, "", 0);
	loc_in_buf = mempcpy(loc_in_buf, player->buffer, 
			strlen((char *)player->buffer));
	loc_in_buf = mempcpy(loc_in_buf, "\n", 1);

	set_player_buffer_replace(player, tmp_buf);

	free(tmp_buf);

	return 2;
}
