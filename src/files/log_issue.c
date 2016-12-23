void log_issue (uint8_t issue_string[]) {
    //struct  tm *tmp;            // for time
    time_t t = time(NULL); 
    if (issue_string[strlen((char*)issue_string)] == '\n') {
        issue_string[strlen((char*)issue_string)] = '\0'; 
    }
    fprintf(stdout, "%lu | %s | %s\n", t, issue_string, strerror(errno));
    fprintf(stderr, "%lu | %s | %s\n", t, issue_string, strerror(errno));
}
