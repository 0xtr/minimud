#include "player_details.h"

static const uint8_t RESERVED_WORDS[10][15] = { "admin", "root", "TABLE", "ADMIN", "Admin", "Administrator" };

int32_t get_existing_player_hash(const int32_t socket)
{
	uint8_t *sqlerr = NULL;
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", get_player_name(socket));
	if (sqlite3_exec(get_playerdb(), (char *)pcheck, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 error! Failed to get player's hash:\n%s\n", sqlite3_errmsg(get_playerdb()));
		sqlite3_free(sqlerr);
		sqlite3_free(pcheck);
		return EXIT_FAILURE;
	}
	sqlite3_free(pcheck);
	if (get_sqlite_rows_count() == 0) {
		fprintf(stdout, "Couldn't retrieve the hash for player %d.\n", socket);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int32_t get_player_coord(const int32_t coord_type, const int32_t socket)
{
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", get_player_name(socket));
	uint8_t *sqlerr = NULL;
	if (sqlite3_exec(get_playerdb(), (char *)pcheck, callback, 0, (char **)sqlerr) != SQLITE_OK) {
	#ifdef DEBUG
		fprintf(stdout, "SQLITE3 error in get_player_coord:\n%s\n", sqlite3_errmsg(get_playerdb()));
	#endif
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}
	sqlite3_free(pcheck);
	if (coord_type == X_COORD_REQUEST) {
		//return player_tmp.x;
	} else if (coord_type == Y_COORD_REQUEST) {
		//return player_tmp.y;
	} else if (coord_type == Z_COORD_REQUEST) {
		//return player_tmp.z;
	}
	return EXIT_FAILURE;
}

int32_t get_next_player_num(void)
{
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS;");
	uint8_t *sqlerr = NULL;

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		#ifdef DEBUG
			fprintf(stdout, "SQLITE3 error in get_next_player_num:\n%s\n", sqlite3_errmsg(get_playerdb()));
		#endif
		return EXIT_FAILURE;
	}

	sqlite3_free(pcheck);
	return get_sqlite_rows_count() + 1;
}

int32_t insert_player(const uint8_t *name, const uint8_t *pw, const int32_t socket)
{
	size_t HASH_LEN = 70;
	uint8_t *sqlerr = NULL;
	uint8_t salt[SALTLEN] = {0};

	for (size_t i = 0; i != 10; ++i) {
		memset(salt, '\0', SALTLEN);
		strcpy((char *)salt, bcrypt_gensalt(10)); // pretty weak
	}

	// hash the salt + password, in that order
	uint8_t *hash_result = calloc(HASH_LEN, sizeof(uint8_t));
	bcrypt_newhash((char *)pw, 10, (char *)hash_result, HASH_LEN);


	// insert the above ^
	// player id in table, name, hash, salt, last ip, x, y, z
	uint8_t *querystr = (uint8_t *)sqlite3_mprintf("INSERT INTO PLAYERS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char)socket, (char *)name, (char *)hash_result, (char *)salt, (char *)get_player_store(socket), "0", "0", "0");

	if (sqlite3_exec(get_playerdb(), (char *)querystr, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE player insert error:\n%s\n", sqlite3_errmsg(get_playerdb()));
		print_to_player(socket, PLAYER_CREATION_FAILED);

		free(hash_result);

		sqlite3_free(sqlerr);
		sqlite3_free(querystr);
		return EXIT_FAILURE;
	}

	sqlite3_free(querystr);
	explicit_bzero(&salt, SALTLEN);
	explicit_bzero(&name, NAMES_MAX);
	explicit_bzero(&pw, NAMES_MAX);
	explicit_bzero(&hash_result, HASH_LEN);

	free(hash_result);

	print_to_player(socket, PLAYER_CREATION_SUCCESS);
	return EXIT_SUCCESS;
}

int32_t lookup_player(const uint8_t *name)
{
	uint8_t *sqlerr = NULL;
	// check for players with the same name 
	uint8_t *pcheck = (uint8_t *)sqlite3_mprintf("SELECT * FROM PLAYERS WHERE name LIKE %Q;", name);

	if (sqlite3_exec(get_playerdb(), (char *)pcheck, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 error in lookup_player:\n%s\n", sqlite3_errmsg(get_playerdb()));
		sqlite3_free(pcheck);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}
	sqlite3_free(pcheck);
	return (get_sqlite_rows_count() != 0) ? 1 : 0;
}

int32_t check_if_name_is_reserved(const int32_t socket, const uint8_t *name)
{
	for (size_t i = 0; i < 10; ++i) {
		if (strcmp((char *)name, (char *)RESERVED_WORDS[i]) == 0) {
			print_to_player(socket, NAME_UNAVAILABLE);
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

int32_t check_if_name_is_valid(const int32_t socket, const uint8_t *name)
{
	if (strlen((char *)get_player_buffer(socket)) > NAMES_MAX || strlen((char *)name) < NAMES_MIN) {
		print_to_player(socket, NAME_NOT_WITHIN_PARAMS);
		return EXIT_FAILURE;
	}
	for (size_t i = 0; i < NAMES_MAX; ++i) {
		if (!isalpha(get_player_buffer(socket)[i]))
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
