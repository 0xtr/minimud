#include "handling_incoming_players.h"

int32_t check_for_highest_socket_num(void)
{
	int32_t fdmax = 0;

	for (size_t j = 0; j < get_num_of_players(); ++j) {
		if (get_player_socket(j) > 0) {
			//FD_SET(get_player_socket(j), &rfds);
			if (get_player_socket(j) > fdmax) {
				fdmax = get_player_socket(j);
			}
		}
	}

	return fdmax;
}

int32_t check_if_player_is_already_online(const size_t pnum)
{
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (strcmp((char*)get_player_pname(pnum), (char*)get_player_pname(i)) == 0 && i != pnum) {
			print_to_player(pnum, PLAYER_ALREADY_ONLINE);
			set_player_wait_state(pnum, THEIR_NAME);
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

// TODO: deprecate and remove
_Bool find_and_set_new_player_struct(const int32_t wfd)
{
	_Bool set = 0;

	for (size_t j = 0;; ++j) {
		if (get_player_in_use(j) != 0)
			continue;

		set_player_in_use(j, true);
		set_player_hold_for_input(j, true);
		set_player_socket(j, wfd);
		set_player_wait_state(j, THEIR_NAME);
		//FD_SET(wfd, &rfds);
		/*
		if (wfd > fdmax) {
			fdmax = wfd;
		}
		*/
		set = true;

		/* this doesn't actually do anything productive right now
		 * returns a local address. maybe because telnet to localhost
		 * or maybe because doing it wrong. to come back to later. 
		 * might be useful if working in future to pass a single IP
		 * to bind instead of 0.0.0.0 */
		struct sockaddr_in addr;
		//socklen_t addr_size = sizeof(struct sockaddr_in); 
		struct ifaddrs *ifap;
		//status = getifaddrs(&ifap);
		//status = getpeername(wfd, player[i].address, &player[i].address_len);
		//strncpy(player[j].holder, inet_ntoa(addr.sin_addr), NAMES_MAX);
		break;
	}

	return set;
}

int32_t handle_existing_pass(const int32_t pnum, const uint8_t *command)
{
	if (get_existing_player_hash(pnum) == -1) {
		print_to_player(pnum, UNABLE_TO_RETRIEVE_CHAR);
		set_player_wait_state(pnum, THEIR_NAME);
		return EXIT_FAILURE;
	}

	if (bcrypt_checkpass(command, get_existing_player_hash(pnum)) == -1) {
		print_to_player(pnum, INCORRECT_PASSWORD);
		set_player_wait_state(pnum, THEIR_NAME);
		return EXIT_FAILURE;
	}

	if (lookup_room(get_player_coord(X_COORD_REQUEST, pnum),
			get_player_coord(Y_COORD_REQUEST, pnum),
			get_player_coord(Z_COORD_REQUEST, pnum), -1) == 0) {
		assert(adjust_player_location(pnum, -1, -1, -1) == EXIT_SUCCESS);
		fprintf(stdout, "[INFO] Moving player %d from a nonexistent room.\n", pnum);
	}

	print_to_player(pnum, SHOWROOM);
	set_player_wait_state(pnum, NO_WAIT_STATE);
	set_player_hold_for_input(pnum, 0);

	return EXIT_SUCCESS;
}

int32_t handle_new_pass(const int32_t pnum, const uint8_t *command)
{
	if (strcmp((char*)command, (char*)get_player_store(pnum)) != 0 && 
	   (strlen((char*)command) != strlen((char*)get_player_store(pnum)))) {
		print_to_player(pnum, MISMATCH_PW_SET);
		set_player_wait_state(pnum, THEIR_NAME);
		return EXIT_FAILURE;
	}

	print_to_player(pnum, ATTEMPT_CREATE_USR);
	if ((insert_player(get_player_pname(pnum), get_player_store(pnum), pnum)) == -1) {
		print_to_player(pnum, PLAYER_CREATION_FAILED);
		shutdown_socket(pnum);
		return EXIT_FAILURE;
	}

	set_player_wait_state(pnum, NO_WAIT_STATE);
	set_player_hold_for_input(pnum, 0);
	print_to_player(pnum, SHOWROOM);
	fprintf(stdout, "Player (num %d), name %s has connected.\n", pnum, get_player_pname(pnum));

	return EXIT_SUCCESS;
}

int32_t set_player_confirm_new_pw(const int32_t pnum, const uint8_t *command)
{
	set_player_store_replace(pnum, command);
	if ((print_to_player(pnum, REQUEST_PW_CONFIRM)) == 0) {
		// TODO: confirm return of print_to_player
		// shutdown_socket
	}

	set_player_wait_state(pnum, THEIR_PASSWORD_NEWFINAL);

	return EXIT_SUCCESS;
}
