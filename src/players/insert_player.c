int32_t insert_player (const uint8_t *pname, const uint8_t *pw, const int32_t socknum) {
    #define HASH_RESULT_LEN 70    // what the hash length is
    int32_t pnum     = getplayernum(socknum);
    uint8_t *sqlerr = NULL;

    // get their player id for the table
    uint8_t pnum_str[3];
    memset(pnum_str, '\0', 3);

    // get the salt, ainsley
    uint8_t salt[SALTLEN];
    for (size_t i = 0; i != 10; ++i) {
        memset(salt, '\0', SALTLEN);
        strcpy(salt, bcrypt_gensalt(10)); // pretty weak
    }

    // hash the salt + password, in that order
    uint8_t *hash_result = calloc(HASH_RESULT_LEN, sizeof(uint8_t));
    bcrypt_hashpass(pw, salt, hash_result, HASH_RESULT_LEN);

    // insert the above ^
    // player id in table, name, hash, salt, last ip, x, y, z
    uint8_t *querystr = sqlite3_mprintf("INSERT INTO PLAYERS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
                        pnum_str, pname, hash_result, salt, (char*)get_player_store(pnum), "0", "0", "0");
    if (sqlite3_exec(get_playerdb(), querystr, callback, 0, &sqlerr) != SQLITE_OK) {
       fprintf(stdout, "SQLITE player insert error:\n%s\n", sqlite3_errmsg(get_playerdb()));
       print_output(pnum, PLAYER_CREATION_FAILED);
       free(hash_result);
       sqlite3_free(sqlerr);
       sqlite3_free(querystr);
       return EXIT_FAILURE;
    }

    sqlite3_free(querystr);
    explicit_bzero(&salt, SALTLEN);
    explicit_bzero(&pname, NAMES_MAX);
    explicit_bzero(&pw, NAMES_MAX);
    explicit_bzero(&hash_result, HASH_RESULT_LEN);
    free(hash_result);

    print_output(socknum, PLAYER_CREATION_SUCCESS);
    return EXIT_SUCCESS;
}
