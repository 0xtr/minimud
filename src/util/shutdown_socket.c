#include "../common.h"
#include "shutdown_socket.h"

int32_t shutdown_socket (const int32_t pnum) {
    if (shutdown(get_player_socket(pnum), SHUT_RDWR) == -1) {
        if (errno != ENOTCONN) {
            log_issue((uint8_t*)"Failed to shutdown a connection.\n");
            return EXIT_FAILURE;
        }
    }
    remove_player_record(pnum);
    return EXIT_SUCCESS;
}
