#pragma once

#include "../common.h"

struct command *is_movement_cmd(const uint8_t *cmd, struct command *info);
struct command *is_system_cmd(const uint8_t *cmd, struct command *info);
struct command *is_room_cmd(const uint8_t *cmd, struct command *info);
struct command *is_info_cmd(const uint8_t *cmd, struct command *info);
struct command *is_travel_cmd(const uint8_t *cmd, struct command *info);

struct command *get_command_info(const uint8_t *cmd);
size_t get_num_of_available_cmds(void);
uint8_t *get_command(const size_t num);
uint8_t *get_movement_str(const int32_t dir);
uint8_t *get_opposite_str(const int32_t dir);

enum MOVEMENT {
	DIR_NORTH,
	DIR_N,
	DIR_EAST,
	DIR_E,
	DIR_SOUTH,
	DIR_S,
	DIR_WEST,
	DIR_W,
	DIR_UP,
	DIR_U,
	DIR_DOWN,
	DIR_D,
	DIR_NORTHEAST,
	DIR_NE,
	DIR_SOUTHEAST,
	DIR_SE,
	DIR_SOUTHWEST,
	DIR_SW,
	DIR_NORTHWEST,
	DIR_NW,
	DIR_NOT
};

enum COMMAND_TYPES {
	MOVEMENT,
	ROOM_CHANGE,
	SYSTEM_ACTION,
	TRAVEL_ACTION,
	INFO_REQUEST,
	COMMAND_NOT = -1
};

enum SYSTEM_ACTION {
	SYS_SAY,
	SYS_QUIT,
	SYS_NOT
};

enum ROOM_CHANGE {
	ROOM_MK,
	ROOM_RM,
	ROOM_SET_NAME,
	ROOM_SET_DESC,
	ROOM_SET_FLAG,
	ROOM_SET_EXIT,
	ROOM_SET_NOT
};

enum INFO_REQUEST {
	INFO_ROOM,
	INFO_ROOM2,
	INFO_COMMANDS,
	INFO_PLAYERS,
	INFO_MAP,
	INFO_NOT
};

enum TRAVEL_ACTION {
	TRAVEL_GOTO,
	TRAVEL_SWAP,
	TRAVEL_NOT
};

struct command {
	enum COMMAND_TYPES type;
	int32_t subtype;
};

struct command *get_empty_command();
