int32_t send_and_ensure (const int32_t player_num, const uint8_t *generic_buf) {
    uint8_t *tmp_buf = calloc(BUFLEN, sizeof(uint8_t));
    size_t bytes, expected = strlen(buf);
    ssize_t rv = 0;

    if (strlen(player[player_num].buffer) > PRINT_LINE_WIDTH) {
        double lines = expected / PRINT_LINE_WIDTH;
        double iptr;
        modff (lines, &iptr);
        lines -= iptr;
        int32_t line_pos  = 0;
        if (lines != 0.0) {
            ++iptr;
        }

        double prcnt = 0;
        // this part formats the lines and tries to prevent word breaks
        // by breaking lines early if above a suitable threshold

        for (size_t iters = iptr; iters != 0; iters--) {
            if (i >= BUFLEN) {
                break;
            }
            if (strlen(tmp_buf) == 0) {
                strcpy(tmp_buf, (char*)"> ");
            } else {
                if (strlen(tmp_buf) + 2 <= BUFLEN) {
                    strncat(tmp_buf, (char*)"> ", 2);
                } else {
                    break;
                }
            }
            for (size_t k = 0; k != PRINT_LINE_WIDTH; ++k) { // track place in line
                if (buf[line_pos] == ' ') {
                    // percentage check
                    prcnt = (k / (double)PRINT_LINE_WIDTH) * 100;
                    if (prcnt >= 85) {
                        ++i;
                        break;
                    }
                    if (strlen(tmp_buf) + 1 <= BUFLEN) {
                        strncat(tmp_buf, (char*)" ", 1);
                    }
                } else if (buf[line_pos] == '\r') {
                    if (strlen(tmp_buf) + 1 <= BUFLEN) {
                        strncat(tmp_buf, (char*)"\r", 1);
                    }
                } else {
                    if (buf[line_posi] != '\n') {
                        if (strlen((char*)tmp_buf) + 1 <= BUFLEN) {
                            strncat(tmp_buf, (char*)&buf[i], 1);
                        }
                    } else {
                        if (strlen((char*)tmp_buf) + 1 <= BUFLEN) {
                            strncat(tmp_buf, (char*)" ", 1);
                        }
                    }
                } 
                ++line_pos;
                if (i == BUFLEN) {
                    break;
                }
            }
            if (iters != 0 && strlen(tmp_buf) + 1 < BUFLEN) {
                strncat(tmp_buf, (char*)"\n", 1);
            }
        }
        tmp_buf[strlen(tmp_buf)] = '\0';
        strncpy(buf, tmp_buf, BUFLEN);
        expected = strlen(buf);
        rv = send(wfd, &buf, expected, 0);
    } else {
        if (buf[0] != '>' && buf[1] != ' ') {
            tmp_buf[0] = '>'; 
            tmp_buf[1] = ' '; 
            strncat(tmp_buf, (char*)player[player_num].buffer, BUFLEN - 2);
            strncpy(buf, (char*)tmp_buf, BUFLEN);
            expected += 2;
        }
        if (buf[strlen(buf)] == '\n') {
            buf[strlen(buf)] = '\0';
        }
        rv = send(wfd, &buf, expected, 0);
    }
    memset(tmp_buf, '\0', BUFLEN - 1);
    free(tmp_buf);
    
    if (rv >= 0) {
        bytes = rv; 
    } else {
        fprintf(stdout, "%s: %zd\n", "Problem sending data.", rv);
        log_issue("Problem sending data.");   
        return EXIT_FAILURE;
    }
    if (bytes == -1) {
        return EXIT_FAILURE; // something errored
    }
    if (bytes == expected) { // sent OK
        strncpy(buf, (char*)"\n", 1);
        rv = send(wfd, &buf, expected, 0);
        return EXIT_SUCCESS; 
    }
    rv = send(wfd, &buf[bytes], strlen(buf) - bytes, 0);
    bytes += rv;
    if (rv == -1) {
        switch (errno) {
            case EINTR:
                rv = send(wfd, &buf[bytes], strlen(buf) - bytes, 0);
                break;
            default:
                log_issue((char*)"Problem sending data to socket.");
                return EXIT_FAILURE;
        }
        bytes += rv;
        fprintf(stdout, "%s %zd %s %zd\n", "sent ", bytes, "expected ", expected);
        if (bytes < expected) {
            rv = send(wfd, &buf[bytes], strlen(buf) - bytes, 0);
        } else if (bytes == expected) {
            CLEAR_BUFFER;
            return EXIT_SUCCESS;
        }
    }
    //return 2; // investigate why 2
    return EXIT_SUCCESS;
}
