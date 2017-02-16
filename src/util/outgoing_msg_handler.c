#include "../common.h"
#define CHARS_FOR_PROMPT_AND_NULL 2
#include "outgoing_msg_handler.h"

static double get_buffer_split_by_line_width(const int32_t expected);
static int32_t check_for_prompt_chars(const int32_t pnum);
static _Bool all_data_was_sent(const int32_t total, const int32_t expected);
static _Bool is_there_another_space(const int32_t pnum, const int32_t buffer_pos, const int32_t plus);
static size_t find_reasonable_line_end(const int32_t pnum, const int32_t buffer_pos);
static _Bool is_space_or_newline(const uint8_t character);
static int32_t send_and_handle_errors(const int32_t pnum, const int32_t expected);

int32_t outgoing_msg_handler(const int32_t pnum)
{
	size_t expected = strlen(get_player_buffer(pnum));
	int32_t buffer_pos = 0;

	if ((CHARS_FOR_PROMPT_AND_NULL + strlen((char*)get_player_buffer(pnum))) <= PRINT_LINE_WIDTH) {
		expected += check_for_prompt_chars(pnum);
		return send_and_handle_errors(pnum, expected);
	}

	const double LINES_REQUIRED_FOR_MSG = get_buffer_split_by_line_width(get_player_buffer(pnum));
	uint8_t *processed_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	for (size_t iters = LINES_REQUIRED_FOR_MSG; iters > 0; iters--) {
		const size_t stop_at_char = find_reasonable_line_end(pnum, buffer_pos);

		if (strlen((char*)processed_buf) == 0) {
			strncpy((char*)processed_buf, get_player_buffer(pnum)[buffer_pos], stop_at_char);
		} else {
			strncat((char*)processed_buf, get_player_buffer(pnum)[buffer_pos], stop_at_char);
		}
		strncat((char*)processed_buf, "\n", BUFFER_LENGTH - strlen((char*)processed_buf) - 1);
		buffer_pos += stop_at_char;
	}

	processed_buf[BUFFER_LENGTH] = '\0';
	set_player_buffer_replace(pnum, processed_buf);
	free(processed_buf);

	return send_and_handle_errors(pnum, expected);
}

static size_t find_reasonable_line_end(const int32_t pnum, const int32_t buffer_pos)
{
	int32_t last_space = 0;

	for (size_t i = 0; i < get_print_line_width(); ++i) {
		if (is_space_or_newline(&get_player_buffer(pnum)[buffer_pos + i]) == true) {
			last_space = i;
			if (is_there_another_space(pnum, buffer_pos, i) == false) {
				break;
			}
		} 
	}

	if (((last_space/(double)get_print_line_width())*100) < 80)
		return get_print_line_width();

	return last_space;
}

static _Bool is_space_or_newline(const uint8_t character)
{
	return character == 32 || character == 10;
}

static _Bool is_there_another_space(const int32_t pnum, const int32_t buffer_pos, const int32_t plus)
{
	for (size_t i = plus+1; i < get_print_line_width(); ++i) {
		if (get_player_buffer(pnum)[buffer_pos + i] == ' ')
			return true;
	}

	return false;
}

static double get_buffer_split_by_line_width(const int32_t expected)
{
	double integral;
	double lines = expected / get_print_line_width();

	printf("lines: %f\n", lines);
	modff (lines, &integral);
	lines -= integral;

	if (lines != 0.0)
		++integral;

	return integral;
}

static int32_t send_and_handle_errors(const int32_t pnum, const int32_t expected)
{
	#define MAX_ATTEMPTS 10
	int32_t returned, total = 0;

	for (int i = 0; i < MAX_ATTEMPTS; ++i) {
		returned = send(get_player_socket(pnum), &get_player_buffer(pnum)[total], expected, 0);
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
				shutdown_socket(pnum);
				break;
			case EMSGSIZE:
				perror("Tried to send a message that was too large");
				exit(EXIT_FAILURE);
			default:
				perror("Failure sending data");
				continue;
		}
	}

	clear_player_buffer(pnum);
	return EXIT_SUCCESS;
}

static _Bool all_data_was_sent(const int32_t total, const int32_t expected)
{
	return total == expected;
}

static int32_t check_for_prompt_chars(const int32_t pnum)
{
	if (get_player_buffer(pnum)[0] == '>' && get_player_buffer(pnum)[1] == ' ')
		return EXIT_SUCCESS; // +0 to expected
	uint8_t *tmp_buf = calloc(BUFFER_LENGTH, sizeof(uint8_t));

	tmp_buf[0] = '>'; 
	tmp_buf[1] = ' '; 

	strncat(tmp_buf, (char*)get_player_buffer(pnum), BUFFER_LENGTH - CHARS_FOR_PROMPT_AND_NULL);
	if (tmp_buf[strlen((char*)tmp_buf)] == '\n')
		tmp_buf[strlen((char*)tmp_buf)] = '\0';

	set_player_buffer_replace(pnum, tmp_buf);

	free(tmp_buf);

	return 2;
}
