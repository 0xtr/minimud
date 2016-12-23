int load_rooms_atomic (void) {
    // if sqlite is used, then the flat text files can be used to load text into the
    // db initially then not used any further
    int     n, retval;
    srandom(time(NULL));

    n = access("rooms/", F_OK);
    assert(n != -1); // this is not necessarily usr fail
    n = access("rooms/CORE.txt", F_OK);
    if (n == -1) { // ok, usr fail
        log_issue("No room data available!");
        fprintf(stdout, "%s\n", "No room data available.");
        exit(EXIT_FAILURE);
    } else if (n >= 0) {
        ssize_t val     = 0;
        size_t  len     = BUFLEN;
        ptr    = malloc(len); // should be first malloc'ed then realloc'ed to fit

        // zero the whole map; 0 == no room here
        memset(map, 0, sizeof(map));
        memset(ptr, '\0', BUFLEN);

        fp_room = fopen("rooms/CORE.txt", "r");
        assert(fp_room != NULL);
        // get the first line from file to make sure we can read it OK
        val = getline(&ptr, &len, fp_room);
        assert(val != -1); // do a check before we start allocating anything

        // create the path
        retval = create_space();
        if (retval > 1) { // reroll ordered
            while (retval != 1) {
                memset(map, 0, sizeof(map));
                retval = create_space();
            }
            // repeating the same initial retval
        } else if (retval == -1) { // critical error, quit
            log_issue("Path generation failure. Exited.");
            fprintf(stdout, "%s\n", "Path generation failure. Exited.");
            exit(EXIT_FAILURE);
        }

        n = fclose(fp_room);
        assert(n == 0);
        free(ptr);
        return 1;
    }
    return 0;
}
