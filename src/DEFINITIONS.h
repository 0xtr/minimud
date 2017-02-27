#pragma once

#define DEBUG

#define BUFFER_LENGTH    512
#define PRINT_LINE_WIDTH 56
#define NAMES_MIN        5
#define NAMES_MAX        45
#define MAX_ROOM_NAME    80

// codes relating to standard commands
#define INVALCMD                 -8500
#define SHOWCMDS                 -8501
#define SHOWROOM                 -8502
#define INVALDIR                 -8503
#define LISTPLAYERS              -8504
#define PROMPT                   -8505
#define MAPIT                    -8506
#define ROOM_ONLY                -8508
#define ALL_PLAYERS              -8509
#define REQUEST_PW_FOR_NEW       -8510
#define REQUEST_PW_FOR_EXISTING  -8511
#define ATTEMPT_CREATE_USR       -8512
#define REQUEST_PW_CONFIRM       -8513
#define MISMATCH_PW_SET          -8514

// codes relating to player creation and handling
#define THEIR_NAME               -8600
#define THEIR_PASSWORD_EXISTING  -8601
#define THEIR_PASSWORD_NEWPRELIM -8602
#define THEIR_PASSWORD_NEWFINAL  -8603
#define USR_CONFIRM_NAME         -8604
#define NO_WAIT_STATE            -8605
#define PLAYER_CREATION_FAILED   -8606
#define PLAYER_CREATION_SUCCESS  -8607
#define PLAYER_ALREADY_ONLINE    -8608
#define INCORRECT_PASSWORD       -8609
#define UNABLE_TO_RETRIEVE_CHAR  -8610
#define NAME_UNAVAILABLE         -8611
#define ALPHANUM_NAMES_ONLY      -8612
#define NAME_TOO_LONG            -8613
#define NAME_TOO_SHORT           -8614
#define NAME_NOT_WITHIN_PARAMS   -8615

// codes for handling coordinates
#define X_COORD_REQUEST          -8650
#define Y_COORD_REQUEST          -8651
#define Z_COORD_REQUEST          -8652

// codes for room-relates commands
#define WAIT_ENTER_NEW_ROOM_NAME       -8700
#define WAIT_CONFIRM_NEW_ROOM_NAME     -8701
#define WAIT_ENTER_NEW_ROOM_DESC       -8702
#define WAIT_CONFIRM_NEW_ROOM_DESC     -8703
#define PRINT_PROVIDE_NEW_ROOM_NAME    -8704
#define PRINT_PROVIDE_NEW_ROOM_DESC    -8705
#define PRINT_ROOM_EXIT_CHANGED        -8706
#define PRINT_ROOM_FLAG_CHANGED        -8707
#define ADJUSTING_ROOM_NAME            -8712
#define ADJUSTING_ROOM_DESC            -8713
#define ADJUSTING_ROOM_EXIT            -8714
#define ADJUSTING_ROOM_FLAG            -8715
#define PRINT_CONFIRM_NEW_ROOM_NAME    -8716
#define PRINT_CONFIRM_NEW_ROOM_DESC    -8717
#define PRINT_ADJUSTMENT_SUCCESSFUL    -8718
#define PRINT_COULDNT_ADJUST_ROOM      -8719
#define PRINT_EXITING_CMD_WAIT         -8720
#define PRINT_ENTER_NEW_ROOM_DIR       -8721
#define PRINT_ENTER_DIR_TO_REMOVE      -8722
#define PRINT_INSUFFICIENT_PERMISSIONS -8723
#define PRINT_REMOVED_FROM_ROOM        -8724
#define PRINT_TOGGLED_ROOM_EXIT        -8725
#define PRINT_COULDNT_TOGGLE_EXIT      -8726
#define PRINT_COULDNT_EXIT_NO_ROOM     -8727

#define PRINT_ROOM_REMOVAL_CHECK       -8730
#define PRINT_ROOM_REMOVAL_CONFIRM     -8731
#define PRINT_ROOM_REMOVAL_SUCCESS     -8732
#define PRINT_ROOM_REMOVAL_FAILURE     -8733
#define PRINT_ROOM_CREATION_GIVE_DIR   -8734
#define PRINT_PROVIDE_ROOM_EXIT_NAME   -8735
#define PRINT_PROVIDE_ROOM_FLAG_NAME   -8736
#define PRINT_ROOM_CREATION_CONFIRMALL -8737
#define PRINT_ROOM_CREATION_CANNOT     -8738
#define PRINT_ROOM_CREATION_SUCCESS    -8739
#define PRINT_ROOM_CREATION_FAILURE    -8740

#define WAIT_ROOM_REMOVAL_CHECK        -8741
#define WAIT_ROOM_REMOVAL_CONFIRM      -8742
#define WAIT_ROOM_CREATION_DIR         -8743
#define WAIT_ROOM_CREATION_NAME        -8744
#define WAIT_ROOM_CREATION_DESC        -8745
#define WAIT_ROOM_CREATION_CONF        -8746
#define WAIT_ENTER_FLAG_NAME           -8747
#define WAIT_ENTER_EXIT_NAME		-8748

#define NORTH_DIR			-9000
#define SOUTH_DIR			-9001
#define EAST_DIR			-9002
#define WEST_DIR			-9003
#define UP_DIR				-9004
#define DOWN_DIR			-9005
#define NORTHEAST_DIR   		-9006
#define SOUTHEAST_DIR   		-9007
#define SOUTHWEST_DIR   		-9008
#define NORTHWEST_DIR   		-9009
#define RETURN_ORIGIN_DIR   		-9010
