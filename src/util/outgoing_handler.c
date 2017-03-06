#include "outgoing_handler.h"

static float get_buffer_split_by_line_width(const int32_t expected);
static int32_t check_for_prompt_chars(const int32_t socket);
static _Bool all_data_was_sent(const int32_t total, const int32_t expected);
static size_t find_reasonable_line_end(const int32_t socket, const int32_t buffer_pos);
static int32_t send_and_handle_errors(const int32_t socket, const int32_t expected);
static size_t num_of_newlines(const int32_t socket);

int32_t outgoing_handler(const int32_t socket)
{
	size_t expected = strlen((char *)get_player_buffer(socket));
	int32_t buffer_pos = 0;

	if ((CHARS_FOR_PROMPT_AND_NULL + strlen((char *)get_player_buffer(socket))) <= PRINT_LINE_WIDTH) {
		expected += check_for_prompt_chars(socket);
		return send_and_handle_errors(socket, expected);
	}

	void *loc_in_buf = NULL;
	const double LINES_REQUIRED_FOR_MSG = 
		get_buffer_split_by_line_width(strlen((char *)get_player_buffer(socket)))
		+ num_of_newlines(socket);

	uint8_t *processed_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	for (size_t iters = 0; iters < LINES_REQUIRED_FOR_MSG; ++iters) {
	const size_t stop_at_char = find_reasonable_line_end(socket, buffer_pos);
	printf("stop at: %lu\n", stop_at_char);

			printf("processed_buf:\n[%s]\n", processed_buf);
		if (strlen((char *)processed_buf) == 0) {
			//loc_in_buf = mempcpy(processed_buf, &get_player_buffer(socket)[buffer_pos], stop_at_char);
			memcpy(&processed_buf[strlen((char *)processed_buf)], &get_player_buffer(socket)[buffer_pos], stop_at_char);
		} else {
			memcpy(&processed_buf[strlen((char *)processed_buf)], &get_player_buffer(socket)[buffer_pos], stop_at_char);
			//loc_in_buf = mempcpy(loc_in_buf, &get_player_buffer(socket)[buffer_pos], stop_at_char);
			// accepting outgoing as incoming, giving valgrind Invalid command buffer
		}
			printf("post processed_buf:\n[%s]\n", processed_buf);

		//loc_in_buf = mempcpy(loc_in_buf, "\n", 1);
		memcpy(&processed_buf[strlen((char *)processed_buf)], "\n", 1);
		buffer_pos += stop_at_char + 1;
		if (buffer_pos > expected)
			break;
	}

	set_player_buffer_replace(socket, processed_buf);
	free(processed_buf);

	return send_and_handle_errors(socket, expected);
}

static size_t num_of_newlines(const int32_t socket)
{
	size_t newlines = 0;
	for (size_t i = 0; i < strlen((char *)get_player_buffer(socket)); ++i) {
		if (get_player_buffer(socket)[i] == '\n')
			++newlines;
	}
	return newlines;
}

static size_t find_reasonable_line_end(const int32_t socket, const int32_t buffer_pos)
{
	int32_t last_space = 0;
	uint8_t *last_match, *substr = calloc(PRINT_LINE_WIDTH, sizeof(uint8_t));
	printf("got their buffer: %s\n", get_player_buffer(socket));
	printf("buffer pos: %d\n", buffer_pos);

	memcpy(substr, &get_player_buffer(socket)[buffer_pos], PRINT_LINE_WIDTH - 20);
	printf("substr: [[%s]]\n", substr);

	if ((last_match = (uint8_t *)strrchr((char *)substr, ' ')) != NULL)
		last_space = last_match - substr;

	if ((last_match = (uint8_t *)strchr((char *)substr, '\n')) != NULL) {
		last_space = last_match - substr;
		free(substr);
		printf("newline last space %d\n", last_space);
		return last_space;
	}

	free(substr);

	if (((last_space/(double)PRINT_LINE_WIDTH)*100) < 80)
		return PRINT_LINE_WIDTH;

	printf("newline last space %d\n", last_space);
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

static int32_t send_and_handle_errors(const int32_t socket, const int32_t expected)
{
	#define MAX_ATTEMPTS 10
	int32_t returned, total = 0;

	for (int i = 0; i < MAX_ATTEMPTS; ++i) {
		returned = send(socket, &get_player_buffer(socket)[total], expected, 0);
		if (returned != -1) {
			if (all_data_was_sent((total += returned), expected) == EXIT_SUCCESS) {
				break;
			} else {
				continue;
			}
		}
		switch (errno) {
			case ENOTCONN:
			case ECONNRESET:
				perror("Socket needs to be terminated; client left");
				shutdown_socket(socket);
				break;
			case EMSGSIZE:
				perror("Tried to send a message that was too large");
				exit(EXIT_FAILURE);
			default:
				perror("Failure sending data");
				continue;
		}
	}

	clear_player_buffer(socket);
	return EXIT_SUCCESS;
}

static _Bool all_data_was_sent(const int32_t total, const int32_t expected)
{
	return total == expected;
}

static int32_t check_for_prompt_chars(const int32_t socket)
{
	if (get_player_buffer(socket)[0] == '>' && get_player_buffer(socket)[1] == ' ')
		return EXIT_SUCCESS; // +0 to expected
	uint8_t *tmp_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	tmp_buf[0] = '>'; 
	tmp_buf[1] = ' '; 

	strncat((char *)tmp_buf, (char *)get_player_buffer(socket), BUFFER_LENGTH - CHARS_FOR_PROMPT_AND_NULL);
	if (tmp_buf[strlen((char *)tmp_buf)] == '\n')
		tmp_buf[strlen((char *)tmp_buf)] = '\0';

	set_player_buffer_replace(socket, tmp_buf);

	free(tmp_buf);

	return 2;
}
