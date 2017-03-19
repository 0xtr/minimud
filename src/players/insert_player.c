#include "../players/insert_player.h"

#define HASH_LENGTH 	70
#define SALT_LENGTH   	50
int32_t insert_player(const uint8_t *name, const uint8_t *pw, const int32_t socket)
{
	uint8_t *sqlerr = NULL;
	uint8_t *salt = calloc(SALT_LENGTH, sizeof(uint8_t));
	const size_t PASSWORD_LEN = (strlen((char *)pw) > BUFFER_LENGTH) ? BUFFER_LENGTH : strlen((char *)pw);
	reset_sqlite_rows_count();

	strcpy((char *)salt, bcrypt_gensalt(10)); // pretty weak
	salt[SALT_LENGTH] = '\0';

	uint8_t *salt_and_pw = calloc(BUFFER_LENGTH + SALT_LENGTH, sizeof(uint8_t));
	void *append_pw = mempcpy(salt_and_pw, salt, strlen((char *)salt));
	append_pw = mempcpy(append_pw, pw, PASSWORD_LEN);
	append_pw = mempcpy(append_pw, "\0", 1);
	
	uint8_t *hash_result = calloc(HASH_LENGTH, sizeof(uint8_t));
	bcrypt_newhash((char *)salt_and_pw, 10, (char *)hash_result, HASH_LENGTH);

	printf("doot 1\n");
	uint8_t *querystr = (uint8_t *)sqlite3_mprintf(
		"INSERT INTO PLAYERS (name, hash, salt, last_ip, x, y, z) "
		"VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char *)name, (char *)hash_result, (char *)salt, "-", "0", "0", "0");
	printf("doot 2\n");

	if (sqlite3_exec(get_playerdb(), (char *)querystr, player_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE player insert error:\n%s\n", sqlite3_errmsg(get_playerdb()));
		print_to_player(socket, PLAYER_CREATION_FAILED);

		memset(salt, '\0', SALT_LENGTH);
		memset(salt_and_pw, '\0', BUFFER_LENGTH + SALT_LENGTH);
		memset(hash_result, '\0', HASH_LENGTH);

		free(salt_and_pw);
		free(salt);
		free(hash_result);
		sqlite3_free(querystr);
		sqlite3_free(sqlerr);

		return EXIT_FAILURE;
	}

	printf("clearing\n");
		memset(salt, '\0', SALT_LENGTH);
		memset(salt_and_pw, '\0', BUFFER_LENGTH + SALT_LENGTH);
		memset(hash_result, '\0', HASH_LENGTH);
	printf("cleared\n");
		free(salt_and_pw);
		free(salt);
		free(hash_result);
		sqlite3_free(querystr);
	printf("freed\n");

	print_to_player(socket, PLAYER_CREATION_SUCCESS);
	printf("returning\n");
	return EXIT_SUCCESS;
}
