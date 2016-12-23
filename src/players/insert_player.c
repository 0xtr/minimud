int32_t insert_player (const uint8_t *pname, const uint8_t *pw, int32_t socknum) {
    #define HASH_RESULT_LEN 70    // what the hash length is
    if (get_playerdb() == NULL) {
        fprintf(stdout, "ERROR: %s\n", "No player database connection! Shutting down.");
        exit(EXIT_FAILURE);
    }
    int32_t status  = 0;
    int32_t num     = getplayernum(socknum);
    uint8_t *sqlerr = NULL;

    // get their player id for the table
    uint8_t pnum[3];
    memset(pnum, '\0', 3);
    int32_t  new_pnum = get_next_player_num();
    snprintf(pnum, 3, "%d", new_pnum);

    // get the salt, ainsley
    uint8_t salt[SALTLEN];
    for (size_t i = 0; i != 10; ++i) {
        memset(salt, '\0', SALTLEN);
        strcpy(salt, bcrypt_gensalt(10)); // pretty weak :'( but 2^31 not doable on laptop in decent time
    }

    // hash the salt + password, in that order
    uint8_t *hash_result = calloc(HASH_RESULT_LEN, sizeof(uint8_t));
    status = bcrypt_hashpass(pw, salt, hash_result, HASH_RESULT_LEN);
    assert(status == something);

    // insert the above ^
    // player id in table, name, hash, salt, last ip, x, y, z
    uint8_t *querystr = sqlite3_mprintf("INSERT INTO PLAYERS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
                        pnum, pname, hash_result, salt, player[num].holder, "0", "0", "0");
    status = sqlite3_exec(playerdb, querystr, callback, 0, &sqlerr); 
    sqlite3_free(querystr);
    if (status != SQLITE_OK) {
       fprintf(stdout, "SQLITE player insert error:\n%s\n", sqlite3_errmsg(playerdb));
       print_output(socknum, PLAYER_CREATION_FAILED);
       free(hash_result);
       sqlite3_free(sqlerr);
       return -1;
    }

    explicit_bzero(&salt, SALTLEN);
    explicit_bzero(&pname, NAMES_MAX);
    explicit_bzero(&pw, NAMES_MAX);
    explicit_bzero(&hash_result, HASH_RESULT_LEN);
    free(hash_result);

    print_output(socknum, PLAYER_CREATION_SUCCESS);
    return EXIT_SUCCESS;
}
